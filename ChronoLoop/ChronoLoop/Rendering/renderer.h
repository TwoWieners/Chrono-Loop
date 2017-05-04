#pragma once
#include <d3d11.h>
#include <d2d1.h>
#include <dwrite.h>
#include "../Common/Math.h"
#include <memory>
#include <vector>
#include <openvr.h>
#include <d2d1_1.h>
#include "Mesh.h"
#include "RenderSet.h"
#include "RenderShape.h"
#include <wrl/client.h>
#include <mutex>

#define num_lights 3

namespace Epoch {

	class Renderer {
	private:
		struct ViewProjectionBuffer {
			matrix4 view, projection;
		} mVPLeftData, mVPRightData;

		static Renderer* sInstance;
		//TODO: Light buffers
		Light * mLData[num_lights];

		// Instance members
		// D3D11 Variables
		Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> mChain;
		Microsoft::WRL::ComPtr<IDXGIFactory1> mFactory;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView> mMainView, mSceneView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mMainViewTexture;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mDSView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mDepthBuffer, mSceneTexture;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplerState;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilState> mTransparentState, mOpaqueState, mTopmostState;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mSceneSRV;
		Microsoft::WRL::ComPtr<ID3D11BlendState> mOpaqueBlendState, mTransparentBlendState;
		D3D11_VIEWPORT mLeftViewport, mRightViewport, mFullViewport;
		HWND mWindow;

		vr::IVRSystem* mVrSystem;
		RenderSet mOpaqueSet, mTransparentSet, mTopmostSet;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mVPBuffer, mPositionBuffer, mSimInstanceBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mLBuffer;
		bool mUseVsync = false;
		//ShadowMap 1 - Directional, 2 - Point, 3 - Spot
		Microsoft::WRL::ComPtr<ID3D11VertexShader> mShadowVS, mShadowVS2;
		Microsoft::WRL::ComPtr<ID3D11PixelShader> mPSST;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView> mSDSView[2];
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mShadowTextures[2];
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mShadowSRV[2];
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSSamplerState;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mPLBufferS, mPLBSDir;
		ViewProjectionBuffer mPLVPB;
		D3D11_VIEWPORT mShadowVP;


		// Blurring variables & Functions
		enum BlurPingPong {
			BlurPingPong_Ping = 0,
			BlurPingPong_Pong
		};
		enum BlurStage {
			BLUR_STAGE_SAMPLE = 0,
			BLUR_STAGE_BLUR
		};
		struct BlurData {
			union {
				struct {
					unsigned int stage;
					float sigma;
					float dx;
					float dy;
				};
				vec4i padding;
			};
			BlurData() {
				stage = 0;
				sigma = 1.0f;
				dx = dy = 0;
			}
		} mBlurData;
		enum BlurTextureSet {
			BlurTextureSet_Ping = 0,
			BlurTextureSet_Pong
		} mBlurTextureSet = BlurTextureSet_Pong; // Default to pong so that the first call to toggle sets it to render to the Ping set.
		Microsoft::WRL::ComPtr<ID3D11Buffer> mBlurStageBuffer;
		void RenderBlurStage(BlurStage _s, float _dx, float _dy);
		void ToggleBlurTextureSet(unsigned int _texturesPerSet, ID3D11RenderTargetView **_rtvs, ID3D11ShaderResourceView **_srvs);
		void SetBlurTexturesDrawback(unsigned int _texturesPerSet, ID3D11RenderTargetView **_drawbacks, ID3D11ShaderResourceView **_srvs);
		float bootlegSigma = 1.0f;
		float bootlegDownsample = 0.5f;
		bool bootlegBlurEnabled = false;

		RenderShape* mScenePPQuad = nullptr, *mSceneScreenQuad = nullptr;
		RenderContext mCurrentContext;

		std::mutex mRendererLock;

		//Pat Added
		//DirectWrite Drawing componets

		void InitializeD3DDevice();
		void InitializeDXGIFactory();
		void InitializeDXGISwapChain(HWND &_win, bool _fullscreen, int _fps,
																	int _width, int _height);
		int mFps = 0;
		float mFrameTime = 0;

		void InitializeViews(int _width, int _height);
		void InitializeBuffers();
		void InitializeSamplerState();
		void InitializeObjectNames();
		void InitializeSceneQuad();
		void SetStaticBuffers();
		void InitializeStates();

		void AttachPrimaryViewports();
		void AttachPrimaryRTVs();

		matrix4 mEyePosLeft, mEyePosRight, mEyeProjLeft, mEyeProjRight, mHMDPos;

		POINT mMouseOrigin;
		bool mIsMouseDown = false;

		matrix4 GetEye(vr::EVREye e);
		matrix4 GetProjection(vr::EVREye e);
		void UpdateViewProjection();
		void UpdateGSBuffers();
		void UpdateLBuffers();
		void RenderShadowMaps(float _delta);
		
		void RenderVR(float _delta);
		void UpdateCamera(float const moveSpd, float const rotSpd, float delta);
		void RenderNoVR(float _delta);
		void ProcessRenderSet();
		void RenderScreenQuad();


		Renderer();
		~Renderer();
	public:
		struct TimeManipulationEffectData {
			vec4f saturationColor;
			vec4f tintColor;
			vec2f ratios;
			vec2f fullRatios;
		};

		static Renderer* Instance();
		static void DestroyInstance();
		// Instance Functions
		bool iInitialize(HWND Window, unsigned int width, unsigned int height,
			bool vsync, int fps, bool fullscreen, float farPlane, float nearPlane,
			vr::IVRSystem* vrsys);
		void ThrowIfFailed(HRESULT hr);
		GhostList<matrix4>::GhostNode* AddOpaqueNode(RenderShape& _node);
		GhostList<matrix4>::GhostNode* AddTransparentNode(RenderShape& _node);
		GhostList<matrix4>::GhostNode* AddTopmostNode(RenderShape& _node);
		void RemoveOpaqueNode(RenderShape& _node);
		void RemoveTransparentNode(RenderShape& _node);
		void RemoveTopmostNode(RenderShape& _node);
		void UpdateOpaqueNodeBuffer(RenderShape& _node, ConstantBufferType _t, unsigned int _index);
		void UpdateTransparentNodeBuffer(RenderShape& _node, ConstantBufferType _t, unsigned int _index);
		void UpdateTopmostNodeBuffer(RenderShape& _node, ConstantBufferType _t, unsigned int _index);
		void Render(float _deltaTime);

		void ClearRenderSet();

		bool BlurTextures(ID3D11Texture2D **_textures, unsigned int _numTextures, float _sigma, float _downsample = 0.5f);

		inline Microsoft::WRL::ComPtr<ID3D11Device>& GetDevice() { return mDevice; }
		inline Microsoft::WRL::ComPtr<ID3D11DeviceContext>& GetContext() { return mContext; }
		inline Microsoft::WRL::ComPtr<IDXGISwapChain>& GetChain() { return mChain; }
		inline Microsoft::WRL::ComPtr<IDXGIFactory1>& GetFactory() { return mFactory; }
		inline Microsoft::WRL::ComPtr<ID3D11RenderTargetView>& GetRTView() { return mMainView; }
		inline Microsoft::WRL::ComPtr<ID3D11DepthStencilView>& GetDSView() { return mDSView; }
		inline Microsoft::WRL::ComPtr<ID3D11Texture2D>& GetRTViewTexture() { return mMainViewTexture; }
		inline HWND GetWindow() { return mWindow; }
		inline RenderShape* GetSceneQuad() { return mScenePPQuad; }
		inline std::mutex& GetRendererLock() { return mRendererLock; }
		inline void SetLight(Light* _light, int _i) { mLData[_i] = _light; }
	};
}
