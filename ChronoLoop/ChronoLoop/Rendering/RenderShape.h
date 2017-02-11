#pragma once
#include "RenderNode.h"
#include "RenderContext.h"

struct ID3D11Buffer;
class Mesh;
struct ID3D11PixelShader;
struct ID3D11VertexShader;

namespace RenderEngine {

	struct RenderShape : public RenderNode {
		ID3D11PixelShader    *pShader = nullptr;
		ID3D11VertexShader   *vShader = nullptr;
		ID3D11Buffer         *mVertexBuffer = nullptr;
		ID3D11Buffer         *mIndexBuffer = nullptr;
		unsigned int         mIndexCount = 0;

		RenderShape();
		RenderShape(Mesh& _mesh);
		void Load(Mesh& _mesh);
		void LoadShaders(char* vertex, char* pixel);

	protected:
		RenderContext mContext;
	};

}