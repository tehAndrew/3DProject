#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <d3d11.h> // Linking has already been done.
#include <DirectXMath.h>

#include "Texture.h"

using namespace DirectX;

struct Vertex {
	float x, y, z;
	float nx, ny, nz;
	float u, v;
};

struct MaterialData {
	XMVECTOR reflectiveColor;
	float gloss;
};

class Model {
	private:
		ID3D11Buffer* mVertexBuffer;
		ID3D11Buffer* mIndexBuffer;
		ID3D11Buffer* mMaterialBuffer;

		UINT32 mVertexAmount;
		UINT32 mIndexAmount;

		float mX;
		float mY;
		float mZ;

		float mScaleX;
		float mScaleY;
		float mScaleZ;

		float mRotX;
		float mRotY;
		float mRotZ;

		MaterialData mMaterial;
		Texture* mTexture;

	public:
		Model();
		virtual ~Model();

		HRESULT defineGeometry(std::vector<Vertex> *vertices, std::vector<unsigned int> *indices, ID3D11Device* device, Texture* texture, MaterialData material);

		void setPos(FXMVECTOR pos);
		void setScale(FXMVECTOR scale);
		void setRot(FXMVECTOR rot);

		XMMATRIX                  getWorldMatrix() const;
		ID3D11ShaderResourceView* getTexture() const;
		ID3D11Buffer*             getMaterial() const;

		void render(ID3D11DeviceContext* deviceContext);
};

#endif