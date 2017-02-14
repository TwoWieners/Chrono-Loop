//#include "stdafx.h"
#include "Renderer.h"
#include <d3d11.h>
#include<d2d1_1.h>
#include <dxgi1_2.h>
#include <openvr.h>
#include "../Common/Logger.h"
#include "Mesh.h"
#include "InputLayoutManager.h"
#include "RenderShape.h"
#include "../Input/VRInputManager.h"
#include "../Input/Controller.h"

using namespace std;
using namespace D2D1;

namespace RenderEngine {

	Renderer* Renderer::sInstance = nullptr;

#pragma region Singleton Functions

	Renderer* Renderer::Instance() {
		if (nullptr == sInstance) {
			sInstance = new Renderer;
		}
		return sInstance;
	}

	void Renderer::DestroyInstance() {
		if (sInstance != nullptr) {
			delete sInstance;
		}
		sInstance = nullptr;
	}

#pragma endregion Singleton Functions

#pragma region Instance Functions


#pragma region Private Functions


	void Renderer::ThrowIfFailed(HRESULT hr) {
		if (FAILED(hr)) {
			throw "Something has gone catastrophically wrong!";
		}
	}

	matrix4 Renderer::GetEye(vr::EVREye e) {
		vr::HmdMatrix34_t matEyeRight = mVrSystem->GetEyeToHeadTransform(e);
		matrix4 matrixObj(
			matEyeRight.m[0][0], matEyeRight.m[1][0], matEyeRight.m[2][0], 0.0,
			matEyeRight.m[0][1], matEyeRight.m[1][1], matEyeRight.m[2][1], 0.0,
			matEyeRight.m[0][2], matEyeRight.m[1][2], matEyeRight.m[2][2], 0.0,
			matEyeRight.m[0][3], matEyeRight.m[1][3], matEyeRight.m[2][3], 1.0f
		);

		return matrixObj.Inverse();
	}

	matrix4 Renderer::GetProjection(vr::EVREye e) {

		vr::HmdMatrix44_t mat = mVrSystem->GetProjectionMatrix(e, 0.1f, 1000);

		return matrix4(
			mat.m[0][0], mat.m[1][0], mat.m[2][0], mat.m[3][0],
			mat.m[0][1], mat.m[1][1], mat.m[2][1], mat.m[3][1],
			mat.m[0][2], mat.m[1][2], mat.m[2][2], mat.m[3][2],
			mat.m[0][3], mat.m[1][3], mat.m[2][3], mat.m[3][3]
		);
	}

	void Renderer::GetMVP(vr::EVREye e, MyBuffer &data, matrix4 world) {
		matrix4 hmd = (Math::FromMatrix(poses[0].mDeviceToAbsoluteTracking));

		matrix4 hmdPos = hmd.Inverse();
		if (e == vr::EVREye::Eye_Left) {
			data.model = Math::MatrixTranspose(world);
			data.view = Math::MatrixTranspose((hmdPos * mEyePosLeft));
			data.projection = Math::MatrixTranspose(mEyeProjLeft);
		} else {
			data.model = Math::MatrixTranspose(world);
			data.view = Math::MatrixTranspose((hmdPos * mEyePosRight));
			data.projection = Math::MatrixTranspose(mEyeProjRight);
		}
	}

	void Renderer::UpdateTrackedPositions() {
		vr::VRCompositor()->WaitGetPoses(poses, vr::k_unMaxTrackedDeviceCount, NULL, 0);
	}

	Renderer::Renderer() {}

	Renderer::Renderer::~Renderer() {
		(*mContext)->Release();
		(*mDSView)->Release();
		(*mDepthBuffer)->Release();
		(*mMainView)->Release();
		(*mFactory)->Release();
		(*mChain)->Release();
		// The device's release has been moved to ChronoLoop.cpp
		constantBluffer->Release();
		(*mMainViewTexture)->Release();
		mContext.reset();
		mDSView.reset();
		mDepthBuffer.reset();
		mMainView.reset();
		mFactory.reset();
		mChain.reset();
		mDevice.reset();

		(*mTextFactory)->Release();
		(*mDevice2D)->Release();
		(*mGIDevice)->Release();
		(*mContext2D)->Release();
		(*mDWrite)->Release();
		(*mTextformat)->Release();
		(*mBrush)->Release();
		(*mBitmap)->Release();

		mTextFactory.reset();
		mDevice.reset();
		mGIDevice.reset();
		mContext2D.reset();
		mDWrite.reset();
		mTextformat.reset();
		mBrush.reset();
		mBitmap.reset();

	}

	void Renderer::InitializeD3DDevice() {
		UINT flags = 0;
	#if _DEBUG
		flags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_BGRA_SUPPORT;
	#endif

		D3D_FEATURE_LEVEL levels[] = {
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
			D3D_FEATURE_LEVEL_9_3,
			D3D_FEATURE_LEVEL_9_2,
			D3D_FEATURE_LEVEL_9_1
		};

		UINT numLevels = ARRAYSIZE(levels);
		D3D_FEATURE_LEVEL levelUsed;

		ID3D11Device *dev;
		ID3D11DeviceContext *ctx;

		ThrowIfFailed(D3D11CreateDevice(
			NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			flags,
			levels,
			numLevels,
			D3D11_SDK_VERSION,
			&dev,
			&levelUsed,
			&ctx
		));

		mDevice = make_shared<ID3D11Device*>(dev);
		mContext = make_shared<ID3D11DeviceContext*>(ctx);
	}

	void Renderer::InitializeDXGIFactory() {
		IDXGIFactory1 *factory;
		ThrowIfFailed(CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&factory));
		mFactory = make_shared<IDXGIFactory1*>(factory);

	}

	void Renderer::InitializeIDWriteFactory() {
		HRESULT hr;
		//create 2dfactory
		ID2D1Factory1 * factory2;
		ThrowIfFailed(D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &factory2));
		sInstance->mTextFactory = make_shared<ID2D1Factory1*>(factory2);

		//createDxgiDevice
		IDXGIDevice* DxgiDevice;
		ThrowIfFailed((*sInstance->GetDevice())->QueryInterface(__uuidof(IDXGIDevice), (void **)&DxgiDevice));
		sInstance->mGIDevice = make_shared<IDXGIDevice*>(DxgiDevice);

		//create device2d 
		ID2D1Device* Device2d;
		ThrowIfFailed(factory2->CreateDevice(*sInstance->mGIDevice, &Device2d));
		sInstance->mDevice2D = make_shared<ID2D1Device*>(Device2d);

		//create context
		ID2D1DeviceContext* context2d;
		ThrowIfFailed((*sInstance->mDevice2D)->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &context2d));
		sInstance->mContext2D = make_shared<ID2D1DeviceContext*>(context2d);

		//Make a bitmap
		D2D1_BITMAP_PROPERTIES1 bitmapProperties =
			BitmapProperties1(
				D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
				PixelFormat(DXGI_FORMAT_R16G16B16A16_FLOAT, D2D1_ALPHA_MODE_IGNORE),
				0,
				0		//defaults to 96
			);

		IDXGISurface* dxgiBackbuffer;
		ThrowIfFailed((*mChain)->GetBuffer(0, IID_PPV_ARGS(&dxgiBackbuffer)));

		ID2D1Bitmap1* bitmap;
		hr = (*mContext2D)->CreateBitmapFromDxgiSurface(dxgiBackbuffer, &bitmapProperties, &bitmap);
		sInstance->mBitmap = make_shared<ID2D1Bitmap1*>(bitmap);

		//Set render target
		(*mContext2D)->SetTarget((*mBitmap));


		//Write Factory initalized
		IDWriteFactory* WriteFactory;
		ThrowIfFailed(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(WriteFactory), reinterpret_cast<IUnknown**>(&WriteFactory)));
		sInstance->mDWrite = make_shared<IDWriteFactory*>(WriteFactory);

		static const WCHAR fontName[] = L"Verdana";
		static const FLOAT fontSize = 50;

		IDWriteTextFormat* WriteFormat;
		ThrowIfFailed(WriteFactory->CreateTextFormat(
			fontName,
			NULL,
			DWRITE_FONT_WEIGHT_NORMAL,
			DWRITE_FONT_STYLE_NORMAL,
			DWRITE_FONT_STRETCH_NORMAL,
			fontSize,
			L"en-us",
			&WriteFormat));
		sInstance->mTextformat = make_shared<IDWriteTextFormat*>(WriteFormat);

		WriteFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
		WriteFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);


		/*D2D1_PIXEL_FORMAT PF = { DXGI_FORMAT_B8G8R8A8_UNORM,D2D1_ALPHA_MODE_IGNORE };

		D2D1_RENDER_TARGET_PROPERTIES RTProp = {
			D2D1_RENDER_TARGET_TYPE_DEFAULT,
			PF,
			0,
			0,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
		};

		ID2D1DCRenderTarget* rt;
		ThrowIfFailed(factory2->CreateDCRenderTarget(&RTProp, &rt));
		sInstance->m2DRenderTarget = make_shared <ID2D1DCRenderTarget*>(rt);*/

		//Brush for the screen
		ID2D1SolidColorBrush* brush;
		ThrowIfFailed((*mContext2D)->CreateSolidColorBrush(D2D1::ColorF(D2D1::ColorF::White, 1.0f), &brush));
		sInstance->mBrush = make_shared<ID2D1SolidColorBrush*>(brush);




	}

	void Renderer::InitializeDXGISwapChain(HWND &_win, bool _fullscreen, int _fps, int _width, int _height) {
		DXGI_SWAP_CHAIN_DESC scDesc;
		memset(&scDesc, 0, sizeof(scDesc));
		scDesc.BufferCount = 1;
		scDesc.BufferDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		scDesc.BufferDesc.Width = _width;
		scDesc.BufferDesc.Height = _height;
		scDesc.BufferDesc.RefreshRate.Numerator = 1;
		scDesc.BufferDesc.RefreshRate.Denominator = _fps;
		scDesc.SampleDesc.Count = 1;
		scDesc.Windowed = !_fullscreen;
		scDesc.OutputWindow = _win;
		scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		scDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

		IDXGISwapChain *chain;

		ThrowIfFailed((*sInstance->mFactory)->CreateSwapChain((*sInstance->mDevice),
			&scDesc,
			&chain));
		sInstance->mChain = make_shared<IDXGISwapChain*>(chain);
	}

	void Renderer::InitializeViews(int _width, int _height) {
		ID3D11Resource *bbuffer;
		ID3D11RenderTargetView *rtv;
		ThrowIfFailed((*mChain)->GetBuffer(0, __uuidof(bbuffer), (void**)(&bbuffer)));
		ThrowIfFailed((*mDevice)->CreateRenderTargetView(bbuffer, NULL, &rtv));
		mMainView = make_shared<ID3D11RenderTargetView*>(rtv);
		mMainViewTexture = make_shared<ID3D11Texture2D*>((ID3D11Texture2D*)bbuffer);

		ID3D11Texture2D *depthTexture;
		ID3D11DepthStencilView *depthView;

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = _width;
		depthStencilDesc.Height = _height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;
		ThrowIfFailed((*mDevice)->CreateTexture2D(&depthStencilDesc, NULL, &depthTexture));
		ThrowIfFailed((*mDevice)->CreateDepthStencilView(depthTexture, NULL, &depthView));
		(*mContext)->OMSetRenderTargets(1, &rtv, depthView);

		mDepthBuffer = make_shared<ID3D11Texture2D*>(depthTexture);
		mDSView = make_shared<ID3D11DepthStencilView*>(depthView);


		// Viewport
		DXGI_SWAP_CHAIN_DESC scd;
		(*mChain)->GetDesc(&scd);
		mViewport.Width = (FLOAT)scd.BufferDesc.Width;
		mViewport.Height = (FLOAT)scd.BufferDesc.Height;
		mViewport.MinDepth = 0;
		mViewport.MaxDepth = 1;
		mViewport.TopLeftX = 0;
		mViewport.TopLeftY = 0;
		(*mContext)->RSSetViewports(1, &mViewport);
	}

	void Renderer::InitializeObjectNames() {
#if _DEBUG
		char deviceName[] = "Main Rendering Device";
		char contextName[] = "Main Context";
		char swapchainName[] = "Main Swapchain";
		char factoryName[] = "3D DXGI Factory";
		char mainViewName[] = "Backbuffer Render Target View";
		char mainViewTexName[] = "Backbuffer Texture";
		char dsvName[] = "Main Depth-Stencil View";
		char dbName[] = "Main Depth Buffer Texture";
		char cbuffName[] = "Temporary Rendering Constant Buffer";

		(*mDevice)->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(deviceName), deviceName);
		(*mContext)->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(contextName), contextName);
		(*mChain)->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(swapchainName), swapchainName);
		(*mFactory)->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(factoryName), factoryName);
		(*mMainView)->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(mainViewName), mainViewName);
		(*mMainViewTexture)->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(mainViewTexName), mainViewTexName);
		(*mDSView)->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(dsvName), dsvName);
		(*mDepthBuffer)->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(dbName), dbName);
		//constantBluffer->SetPrivateData(WKPDID_D3DDebugObjectName, ARRAYSIZE(cbuffName), cbuffName);
#endif
	}

	void Renderer::RenderVR() {
		(*mContext)->ClearDepthStencilView((*mDSView), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0);
		UpdateTrackedPositions();
		vr::VRCompositor()->CompositorBringToFront();
		float color[4] = { 0.3f, 0.3f, 1, 1 };
		for (int i = 0; i < 2; ++i) {
			vr::EVREye currentEye;
			if (i == 0) {
				currentEye = vr::EVREye::Eye_Left;
			} else {
				currentEye = vr::EVREye::Eye_Right;
				(*mContext)->ClearRenderTargetView((*mMainView), color);
				(*mContext)->ClearDepthStencilView((*mDSView), D3D11_CLEAR_FLAG::D3D11_CLEAR_DEPTH | D3D11_CLEAR_FLAG::D3D11_CLEAR_STENCIL, 1.0f, 0);
			}
			MyBuffer data;
			GetMVP(currentEye, data, Math::MatrixTranslation(0, 1, -1));
			(*mContext)->UpdateSubresource(constantBluffer, 0, nullptr, (void*)&data, 0, 0);
			(*mContext)->VSSetConstantBuffers(0, 1, &constantBluffer);

			processRenderSet();

			vr::Texture_t submitTexture = { (void*)(*mMainViewTexture), vr::TextureType_DirectX, vr::ColorSpace_Auto };
			vr::VRCompositor()->Submit(currentEye, &submitTexture);
		}


		// Bootleg load the controller model.
		if (mControllerModel.mIndexCount == 0) {
			vr::RenderModel_t *vrControllerModel;
			if (vr::VRRenderModels()->LoadRenderModel_Async("vr_controller_vive_1_5", &vrControllerModel) == vr::VRRenderModelError_None) {
				Mesh controller;
				controller.Load(vrControllerModel);
				controller.Invert();
				mControllerModel.Load(controller);
				mControllerModel.SetShaders(ePS_BASIC, eVS_BASIC);
				AddNode(&mControllerModel);
				vr::VRRenderModels()->FreeRenderModel(vrControllerModel);
			}
		}

	}

	void Renderer::RenderNoVR() {

	}

	void Renderer::processRenderSet() {
		const RenderNode* head = mRenderSet.GetHead();
		while (head != nullptr) {
			if (head->mType == RenderNode::RenderNodeType::Context) {
				((RenderContext*)head)->Apply();
			} else if (head->mType == RenderNode::RenderNodeType::Shape) {
				((RenderShape*)head)->Render();
			}
			head = head->GetNext();
		}
	}

#pragma endregion Private Functions

#pragma region Public Functions

	void Renderer::AddNode(RenderShape *node) {
		mRenderSet.AddNode(node, &node->GetContext());
	}

	bool Renderer::Initialize(HWND _Window, unsigned int _width, unsigned int _height, bool _vsync, int _fps, bool _fullscreen, float _farPlane, float _nearPlane, vr::IVRSystem * _vrsys) {
		mWindow = make_shared<HWND>(_Window);
		mVrSystem = _vrsys;

		int rtvWidth = _width;
		int rtvHeight = _height;

		if (mVrSystem) {
			uint32_t texWidth, texHeight;
			mVrSystem->GetRecommendedRenderTargetSize(&texWidth, &texHeight);
			SystemLogger::GetLog() << "According to VR, the view of our headset is " << texWidth << "x" << texHeight << std::endl;
			SystemLogger::GetLog() << "The screen will probably look bad. We're just using one render target view currently, and it gets set to the VR headset's recommended resolution when it's plugged in.\n" <<
				"We should account for that later." << std::endl;
			rtvWidth = (int)texWidth;
			rtvHeight = (int)texHeight;


			mEyePosLeft = GetEye(vr::EVREye::Eye_Left);
			mEyePosRight = GetEye(vr::EVREye::Eye_Right);
			mEyeProjLeft = GetProjection(vr::EVREye::Eye_Left);
			mEyeProjRight = GetProjection(vr::EVREye::Eye_Right);
		}

		InitializeD3DDevice();
		InitializeDXGIFactory();
		InitializeDXGISwapChain(_Window, _fullscreen, _fps, rtvWidth, rtvHeight);
		InitializeViews(rtvWidth, rtvHeight);
#if _DEBUG
		InitializeObjectNames();
#endif

		//PatAdded
		InitializeIDWriteFactory();
		InitializeDXGISwapChain(_Window, _fullscreen, _fps, _width, _height);
		InitializeViews(_width, _height);
		mUseVsync = _vsync;

		//Model loading
		mBox.Load("../Resources/Cube.obj", true, ePS_TEXTURED, eVS_TEXTURED);
		mBox.AddTexture("../Resources/cube_texture.png", eTEX_DIFFUSE);
		AddNode(&mBox);


		CD3D11_BUFFER_DESC desc(sizeof(MyBuffer), D3D11_BIND_CONSTANT_BUFFER);
		(*mDevice)->CreateBuffer(&desc, nullptr, &constantBluffer);

		//// Print out the render model names available from openVR.
		//char buffer[2048];
		//for (int i = 0; i < vr::VRRenderModels()->GetRenderModelCount(); ++i) {
		//	vr::VRRenderModels()->GetRenderModelName((uint32_t)i, buffer, 2048);
		//	SystemLogger::GetLog() << buffer << std::endl;
		//}
		return true;
	}

#pragma endregion Singleton Functions

#pragma region Instance Functions


	void Renderer::Render() {
		float color[4] = { 0.3f, 0.3f, 1, 1 };
		(*mContext)->ClearRenderTargetView((*mMainView), color);
		if (nullptr == mVrSystem) {
			RenderNoVR();
		} else {
			RenderVR();
		}
		(*mChain)->Present(mUseVsync ? 1 : 0, 0);

		//Fix this 
		//float color[4] = { 0.3f, 0.3f, 1, 1 };
		(*mContext)->ClearRenderTargetView((*mMainView), color);
		(*mContext)->OMSetRenderTargets(1, &(*mMainView), *mDSView);

		D2D1_TAG t1 = NULL;
		D2D1_TAG t2 = NULL;

		static const WCHAR sc_helloWorld[] = L"IT JUST WENT OFF";

		// Retrieve the size of the render target.
		D2D1_SIZE_F renderTargetSize = (*mContext2D)->GetSize();

		(*mContext2D)->BeginDraw();
		(*mContext2D)->SetTransform(D2D1::Matrix3x2F::Identity());
		
		(*mContext2D)->SetAntialiasMode(D2D1_ANTIALIAS_MODE_ALIASED);
		(*mContext2D)->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_ALIASED);

		(*mContext2D)->DrawText(
			sc_helloWorld,
			ARRAYSIZE(sc_helloWorld) - 1,
			(*mTextformat),
			D2D1::RectF(0, 0, renderTargetSize.width, renderTargetSize.height),
			(*mBrush)
		);
		HRESULT hr;
		hr= (*mContext2D)->EndDraw(&t1, &t2);

		(*mChain)->Present(0, 0);
	}

#pragma endregion Public Functions

#pragma endregion Instance Functions

}