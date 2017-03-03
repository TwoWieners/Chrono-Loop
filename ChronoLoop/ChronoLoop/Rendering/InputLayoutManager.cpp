//#include "stdafx.h"
#include <d3d11.h>
#include "InputLayoutManager.h"
#include "Renderer.h"
#include "../Common/FileIO.h"


namespace Epoch {

	InputLayoutManager* InputLayoutManager::sInstance = nullptr;

	InputLayoutManager::InputLayoutManager() {
		this->Initialize();
	}

	void InputLayoutManager::Initialize() {
		char* buffer = nullptr;
		int bufferSize = 0;
		D3D11_INPUT_ELEMENT_DESC PosDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		IO::LoadBytes("VertexLayout_POS_VS.cso", &buffer, bufferSize);
		Renderer::Instance()->GetDevice()->CreateInputLayout(PosDesc, ARRAYSIZE(PosDesc), buffer, bufferSize, &mInputLayouts[eVERT_POS]);
		delete[] buffer;
		D3D11_INPUT_ELEMENT_DESC PosColorDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		IO::LoadBytes("VertexLayout_POSCOLOR_VS.cso", &buffer, bufferSize);
		Renderer::Instance()->GetDevice()->CreateInputLayout(PosColorDesc, ARRAYSIZE(PosColorDesc), buffer, bufferSize, &mInputLayouts[eVERT_POSCOLOR]);
		delete[] buffer;
		D3D11_INPUT_ELEMENT_DESC PosTexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		IO::LoadBytes("VertexLayout_POSTEX_VS.cso", &buffer, bufferSize);
		Renderer::Instance()->GetDevice()->CreateInputLayout(PosTexDesc, ARRAYSIZE(PosTexDesc), buffer, bufferSize, &mInputLayouts[eVERT_POSTEX]);
		delete[] buffer;
		D3D11_INPUT_ELEMENT_DESC PosNormTexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		IO::LoadBytes("VertexLayout_POSNORMTEX_VS.cso", &buffer, bufferSize);
		Renderer::Instance()->GetDevice()->CreateInputLayout(PosNormTexDesc, ARRAYSIZE(PosNormTexDesc), buffer, bufferSize, &mInputLayouts[eVERT_POSNORMTEX]);
		delete[] buffer;
		D3D11_INPUT_ELEMENT_DESC PosNormTanTexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 1, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		IO::LoadBytes("VertexLayout_POSNORMTANTEX_VS.cso", &buffer, bufferSize);
		Renderer::Instance()->GetDevice()->CreateInputLayout(PosNormTanTexDesc, ARRAYSIZE(PosNormTanTexDesc), buffer, bufferSize, &mInputLayouts[eVERT_POSNORMTANTEX]);
		delete[] buffer;
		D3D11_INPUT_ELEMENT_DESC PosBoneWeightDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		IO::LoadBytes("VertexLayout_POSBONEWEIGHT_VS.cso", &buffer, bufferSize);
		Renderer::Instance()->GetDevice()->CreateInputLayout(PosBoneWeightDesc, ARRAYSIZE(PosBoneWeightDesc), buffer, bufferSize, &mInputLayouts[eVERT_POSBONEWEIGHT]);
		delete[] buffer;
		D3D11_INPUT_ELEMENT_DESC PosBoneWeightNormTexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		IO::LoadBytes("VertexLayout_POSBONEWEIGHTNORMTEX_VS.cso", &buffer, bufferSize);
		Renderer::Instance()->GetDevice()->CreateInputLayout(PosBoneWeightNormTexDesc, ARRAYSIZE(PosBoneWeightNormTexDesc), buffer, bufferSize, &mInputLayouts[eVERT_POSBONEWEIGHTNORMTEX]);
		delete[] buffer;
		D3D11_INPUT_ELEMENT_DESC PosBoneWeightNormTanTexDesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONEINDICES", 0, DXGI_FORMAT_R32G32B32A32_SINT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "BONEWEIGHTS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TANGENT", 1, DXGI_FORMAT_R32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};
		IO::LoadBytes("VertexLayout_POSBONEWEIGHTNORMTANTEX_VS.cso", &buffer, bufferSize);
		Renderer::Instance()->GetDevice()->CreateInputLayout(PosBoneWeightNormTanTexDesc, ARRAYSIZE(PosBoneWeightNormTanTexDesc), buffer, bufferSize, &mInputLayouts[eVERT_POSBONEWEIGHTNORMTANTEX]);
		delete[] buffer;
	}

	InputLayoutManager::~InputLayoutManager() {
		for (int i = 0; i < VertFormat::eVERT_MAX; i++) {
			mInputLayouts[i]->Release();
		}
	}

	InputLayoutManager & InputLayoutManager::Instance() {
		if (!sInstance)
			sInstance = new InputLayoutManager();
		return *sInstance;
	}

	void InputLayoutManager::DestroyInstance() {
		if (sInstance)
			delete sInstance;
	}

	void InputLayoutManager::ApplyLayout(VertFormat v) {
		Renderer::Instance()->GetContext()->IASetInputLayout(mInputLayouts[v]);
		Renderer::Instance()->GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	}
}
