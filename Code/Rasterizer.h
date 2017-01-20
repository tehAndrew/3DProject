/*
---------------------------------------------------------------------------
Rasterizer.h

The rasterizer class encapsulates all Com objects and handels the rendering
of the project.
---------------------------------------------------------------------------
*/

#ifndef RASTERIZER_H
#define RASTERIZER_H

#include <assert.h>
#include <windows.h>
#include <DirectXMath.h>
#include <d3d11.h>
#include <d3dcompiler.h>

#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "d3dcompiler.lib")

using namespace DirectX;

class Rasterizer {
	private:
		struct Vertex {
			float x, y, z;
			float r, g, b;
		};

		static bool instantiated;

		HWND hostWndHandle;

		// COM Objects
		ID3D11Device*           gDevice;
		ID3D11DeviceContext*    gDeviceContext;
		IDXGISwapChain*         gSwapChain;
		ID3D11RenderTargetView* gRenderTargetView;
		ID3D11DepthStencilView* gDepthStencilView;
		ID3D11Texture2D*        gDepthStencilBuffer;
		ID3D11Buffer*           gVertexBuffer;
		ID3D11Buffer*           gConstBuffer;
		ID3D11InputLayout*      gVertexLayout;
		ID3D11VertexShader*     gVertexShader;
		ID3D11GeometryShader*   gGeometryShader;
		ID3D11PixelShader*      gPixelShader;

		// Matrices
		struct Matrices {
			XMMATRIX worldMatrix;
			XMMATRIX viewMatrix;
			XMMATRIX projMatrix;
		};
		Matrices matrices;

	public:
		Rasterizer();
		virtual ~Rasterizer();

		HRESULT initDirect3D(HWND hWnd, int clientWidth, int clientHeight);

		void setMatrices(FXMMATRIX worldMatrix, FXMMATRIX viewMatrix, FXMMATRIX projMatrix);

		HRESULT render();
};
#endif