/*
---------------------------------------------------------------------------
Renderer3D.h

The Renderer3D class encapsulates all Com objects and handels the rendering
of the project.
---------------------------------------------------------------------------
*/

#ifndef RENDERER3D_H
#define RENDERER3D_H

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

#define BUFFER_AMOUNT 4

#include <vector>
#include <windows.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "FirstPersonCamera.h"
#include "PointLight.h"
#include "ModelCreator.h"
#include "TextureManager.h"

using namespace DirectX;

class Renderer3D {
	private:
		HWND mHostWndHandle;
		int   mClientWidth;
		int   mClientHeight;
		float mNearPlane;
		float mFarPlane;

		// COM Objects
		ID3D11Device*        mDevice;
		ID3D11DeviceContext* mDeviceContext;
		IDXGISwapChain*      mSwapChain;
		ID3D11Buffer*        mMatrixBuffer;
		ID3D11Buffer*        mLightBuffer;
		ID3D11Buffer*        mCameraBuffer;
		ID3D11Buffer*        mFrustumBuffer;
		D3D11_VIEWPORT mSceneViewPort;

		// Sampler states
		ID3D11SamplerState* mSamplerState;
		ID3D11SamplerState* mShadowSamplerState;

		// For the geometry pass.
		ID3D11Texture2D*          mRenderBuffers[BUFFER_AMOUNT];
		ID3D11RenderTargetView*   mRenderTargetViews[BUFFER_AMOUNT];
		ID3D11ShaderResourceView* mShaderResourceViews[BUFFER_AMOUNT];
		ID3D11InputLayout*        mInputLayoutGp;
		ID3D11VertexShader*       mVertexShaderGp;
		ID3D11GeometryShader*     mGeometryShaderGp;
		ID3D11PixelShader*        mPixelShaderGp;
		ID3D11DepthStencilView*   mDepthStencilView;
		ID3D11Texture2D*          mDepthStencilBuffer;

		// For the shadow pass.
		ID3D11InputLayout*    mInputLayoutSp;
		ID3D11VertexShader*   mVertexShaderSp;
		ID3D11PixelShader*    mPixelShaderSp;

		// For the light pass.
		ID3D11RenderTargetView* mBackBufferRtv;
		ID3D11InputLayout*      mInputLayoutLp;
		ID3D11VertexShader*     mVertexShaderLp;
		ID3D11PixelShader*      mPixelShaderLp;
		ID3D11Buffer*           mVertexBufferLp;

		// Light pass vertex.
		struct VertexLp {
			float x, y, z;
			float u, v;
		};

		// Constant Buffer data
		struct MatrixData {
			XMMATRIX worldMatrix;
			XMMATRIX viewMatrix;
			XMMATRIX projMatrix;
		};
		MatrixData mMatrixData;

		struct LightData {
			XMVECTOR ambientColor;
			XMVECTOR diffuseColor;
			XMVECTOR lightPos;
			float    attenuation;
		};
		LightData mLightData;

		struct CameraData {
			XMVECTOR cameraPos;
		};
		CameraData mCameraData;

		struct FrustumData {
			float  FarPlane;
			float  Pad1, Pad2, Pad3;
		};
		FrustumData mFrustumData;

		FirstPersonCamera*   mCamera;
		PointLight*          mPointLight;
		std::vector<Model*>* mModels;
		TextureManager*      mTextureManager;

		HRESULT initGeometryShaderPass(int clientWidth, int clientHeight);
		HRESULT initShadowMapShaderPass();
		HRESULT initLightShaderPass(int clientWidth, int clientHeight);

		HRESULT renderGeometryPass();
		HRESULT renderShadowMapPass();
		HRESULT renderLightPass();

		void setShaders(ID3D11InputLayout* inputLayout, ID3D11VertexShader* vs, ID3D11HullShader* hs, ID3D11DomainShader* ds, ID3D11GeometryShader* gs, ID3D11PixelShader* ps);

	public:
		Renderer3D();
		virtual ~Renderer3D();

		HRESULT initDirect3D(HWND hWnd, int clientWidth, int clientHeight, float nearPlane, float farPlane);

		FirstPersonCamera*   getCamera()     const;
		PointLight*          getPointLight() const;
		std::vector<Model*>* getModelArray() const;
		Texture* getTexture(wchar_t* filename);

		void createModelFromFile(wchar_t* filename, FXMVECTOR pos, FXMVECTOR rot, FXMVECTOR scale);
		void createFloor(FXMVECTOR pos, FXMVECTOR rot, float side, float detail, FXMVECTOR reflectiveColor, float gloss, Texture* texture);
		void createCube(FXMVECTOR pos, FXMVECTOR rot, float side, Texture* texture);

		HRESULT render();
};
#endif