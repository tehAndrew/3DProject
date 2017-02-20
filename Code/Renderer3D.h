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

#include <vector>
#include <windows.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#include "FirstPersonCamera.h"
#include "ModelCreator.h"
#include "TextureManager.h"

using namespace DirectX;

class Renderer3D {
	private:
		HWND mHostWndHandle;

		// COM Objects
		ID3D11Device*           mDevice;
		ID3D11DeviceContext*    mDeviceContext;
		IDXGISwapChain*         mSwapChain;
		ID3D11RenderTargetView* mRenderTargetView;
		ID3D11DepthStencilView* mDepthStencilView;
		ID3D11Texture2D*        mDepthStencilBuffer;
		ID3D11Buffer*           mMatrixBuffer;
		ID3D11Buffer*           mLightBuffer;
		ID3D11Buffer*           mCameraBuffer;
		ID3D11InputLayout*      mVertexLayout;
		ID3D11VertexShader*     mVertexShader;
		ID3D11GeometryShader*   mGeometryShader;
		ID3D11PixelShader*      mPixelShader;

		// Matrices
		struct MatrixData {
			XMMATRIX worldMatrix;
			XMMATRIX viewMatrix;
			XMMATRIX projMatrix;
		};
		MatrixData mMatrixData;

		struct LightData {
			XMVECTOR ambientColor;
			XMVECTOR diffuseColor;
			XMVECTOR specularColor;
			XMVECTOR lightPos;
			float    specPow;
		};
		LightData mLightData;

		struct CameraData {
			XMFLOAT4 cameraPos;
		};
		CameraData mCameraData;

		FirstPersonCamera*   mCamera;
		std::vector<Model*>* mModels;
		TextureManager*      mTextureManager;

	public:
		Renderer3D();
		virtual ~Renderer3D();

		FirstPersonCamera*   getCamera() const;
		std::vector<Model*>* getModelArray() const;

		void createModelFromFile(wchar_t* filename, FXMVECTOR pos, FXMVECTOR rot, FXMVECTOR scale, Texture* texture);
		void createFloor(FXMVECTOR pos, FXMVECTOR rot, float side, Texture* texture);
		void createCube(FXMVECTOR pos, FXMVECTOR rot, float side, Texture* texture);
		void createTexture(wchar_t* filename, std::string name);

		Texture* getTexture(std::string texture) const;

		HRESULT initDirect3D(HWND hWnd, int clientWidth, int clientHeight);

		HRESULT render();
};
#endif