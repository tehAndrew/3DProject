#ifndef LIGHT_H
#define LIGHT_H

#include <d3d11.h>
#include <DirectXMath.h>

using namespace DirectX;

class PointLight {
	private:
		float mTick;

		ID3D11Buffer* mMatrixBuffer;

		struct ShadowMatrixData {
			XMMATRIX worldMatrix;
			XMMATRIX viewMatrix;
			XMMATRIX projMatrix;
		};
		ShadowMatrixData mMatrices;

		XMFLOAT4X4 mViewMatrices[6];
		XMFLOAT4X4 mProjMatrix;

		XMFLOAT4 mPos;

		XMFLOAT4 mAmbientColor;
		XMFLOAT4 mDiffuseColor;
		float	 mAttenuation;

		ID3D11ShaderResourceView* mShadowMapSrv;
		ID3D11DepthStencilView*  mShadowMapDsvs[6];
		D3D11_VIEWPORT mViewPort;

		const int mShadowMapRes = 2048;

	public:
		PointLight();
		virtual ~PointLight();

		HRESULT initPointLight(ID3D11Device* device, float nearPlane, float farPlane);
		HRESULT mapMatrices(ID3D11DeviceContext* deviceContext, FXMMATRIX worldMatrix, int viewIndex);
		void setPos(FXMVECTOR pos);
		void setAmbientColor(FXMVECTOR ambientColor);
		void setDiffuseColor(FXMVECTOR diffuseColor);
		void setAttenuation(float attenuation);

		FXMVECTOR getPos() const;
		FXMVECTOR getAmbientColor() const;
		FXMVECTOR getDiffuseColor() const;
		float     getAttenuation() const;
		D3D11_VIEWPORT getViewPort() const;
		ID3D11Buffer*  getMatrixBuffer() const;

		ID3D11ShaderResourceView* getShadowMapSrv() const;
		ID3D11DepthStencilView*   getShadowMapDsv(int index) const;

		void update(float dt);
};

#endif