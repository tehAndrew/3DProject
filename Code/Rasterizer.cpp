#include "Rasterizer.h"

bool Rasterizer::instantiated = false;

Rasterizer::Rasterizer() {
	assert(!instantiated);
	instantiated = true;

	gDevice             = nullptr;
	gDeviceContext      = nullptr;
	gSwapChain          = nullptr;
	gRenderTargetView   = nullptr;
	gDepthStencilView   = nullptr;
	gDepthStencilBuffer = nullptr;
	gVertexBuffer       = nullptr;
	gConstBuffer        = nullptr;
	gVertexLayout       = nullptr;
	gVertexShader       = nullptr;
	gGeometryShader     = nullptr;
	gPixelShader        = nullptr;
}

Rasterizer::~Rasterizer() {
	if (gDevice != nullptr)
		gDevice->Release();

	if (gDeviceContext != nullptr)
		gDeviceContext->Release();

	if (gSwapChain != nullptr)
		gSwapChain->Release();

	if (gRenderTargetView != nullptr)
		gRenderTargetView->Release();

	if (gDepthStencilView != nullptr)
		gDepthStencilView->Release();

	if (gDepthStencilBuffer != nullptr)
		gDepthStencilBuffer->Release();

	if (gVertexBuffer != nullptr)
		gVertexBuffer->Release();

	if (gConstBuffer != nullptr)
		gConstBuffer->Release();

	if (gVertexLayout != nullptr)
		gVertexLayout->Release();

	if (gVertexShader != nullptr)
		gVertexShader->Release();

	if (gGeometryShader != nullptr)
		gGeometryShader->Release();

	if (gPixelShader != nullptr)
		gPixelShader->Release();

	instantiated = false;
}

HRESULT Rasterizer::initDirect3D(HWND hWnd, int clientWidth, int clientHeight) {
	HRESULT hr;

	// Set window handle
	hostWndHandle = hWnd;

	// Create matrices.
	// Initialize constant buffer data
	m.worldMatrix = XMMatrixIdentity();

	// Set the view matrix
	XMVECTOR pos = XMVectorSet(0.0f, 0.0f, -2.0f, 1.0f);
	XMVECTOR target = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 1.0f);

	m.viewMatrix = XMMatrixLookAtLH(pos, target, up);

	// Set the projection matrix
	float fov = XM_PI * 0.45f;
	float nearPlane = 0.1f;
	float farPlane = 20.0f;
	float aspectRatio = (float) clientWidth / (float) clientHeight;

	m.projMatrix = XMMatrixPerspectiveFovLH(fov, aspectRatio, nearPlane, farPlane);

	m.worldMatrix = XMMatrixTranspose(m.worldMatrix);
	m.viewMatrix = XMMatrixTranspose(m.viewMatrix);
	m.projMatrix = XMMatrixTranspose(m.projMatrix);

	// Describe the swapchain.
	DXGI_SWAP_CHAIN_DESC scDesc;
	ZeroMemory(&scDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	scDesc.BufferCount = 1;                                    // one back buffer
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scDesc.OutputWindow = hostWndHandle;                             // the window to be used
	scDesc.SampleDesc.Count = 4;                               // how many multisamples
	scDesc.Windowed = true;

	// Try to create a device, device context and a swap chain.
	hr = D3D11CreateDeviceAndSwapChain(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		NULL,
		NULL,
		NULL,
		NULL,
		D3D11_SDK_VERSION,
		&scDesc,
		&gSwapChain,
		&gDevice,
		NULL,
		&gDeviceContext
	);

	if (FAILED(hr)) {
		MessageBox(hostWndHandle, L"Device context and swap chain could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Describe the texture to be used in the depth/stencil buffer
	D3D11_TEXTURE2D_DESC tDesc;
	ZeroMemory(&tDesc, sizeof(D3D11_TEXTURE2D_DESC));

	tDesc.Width            = clientWidth;
	tDesc.Height           = clientHeight;
	tDesc.MipLevels        = 1;
	tDesc.ArraySize        = 1;
	tDesc.Format           = DXGI_FORMAT_D24_UNORM_S8_UINT;
	tDesc.SampleDesc.Count = 4;
	tDesc.Usage            = D3D11_USAGE_DEFAULT;
	tDesc.BindFlags        = D3D11_BIND_DEPTH_STENCIL;

	// Create the depth/stencil buffer.
	if (FAILED(hr = gDevice->CreateTexture2D(&tDesc, 0, &gDepthStencilBuffer))) {
		MessageBox(hostWndHandle, L"Depth/stencil buffer could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create the depth/stencil view.
	if (FAILED(hr = gDevice->CreateDepthStencilView(gDepthStencilBuffer, 0, &gDepthStencilView))) {
		MessageBox(hostWndHandle, L"Depth/stencil view could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Get the address of the back buffer.
	ID3D11Texture2D* backBuffer = nullptr;
	if (FAILED(hr = gSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer))) {
		MessageBox(hostWndHandle, L"Back buffer could not be fetched.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	// Create the render target view.
	if (FAILED(hr = gDevice->CreateRenderTargetView(backBuffer, NULL, &gRenderTargetView))) {
		MessageBox(hostWndHandle, L"Back buffer could not be fetched.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}
	// We don't need the texture for the backbuffer anymore.
	backBuffer->Release();

	// Set render target view and depth/stencil view.
	gDeviceContext->OMSetRenderTargets(1, &gRenderTargetView, gDepthStencilView);

	// Describe view port.
	D3D11_VIEWPORT viewPort;
	viewPort.Width    = (float) clientWidth;
	viewPort.Height   = (float) clientHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;

	// Set view port.
	gDeviceContext->RSSetViewports(1, &viewPort);

	// Compile Vertex Shader
	ID3DBlob* vS = nullptr;
	hr = D3DCompileFromFile(
		L"VertexShader.hlsl", // filename
		nullptr,		                // optional macros
		nullptr,		                // optional include files
		"VS_Main",		                // entry point
		"vs_5_0",		                // shader model (target)
		0,				                // shader compile options
		0,				                // effect compile options
		&vS,			                // double pointer to ID3DBlob		
		nullptr		
	);

	if (FAILED(hr)) {
		MessageBox(hostWndHandle, L"Vertex shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Compile Geometry Shader
	ID3DBlob* gS = nullptr;
	hr = D3DCompileFromFile(
		L"GeometryShader.hlsl",           // filename
		nullptr,		                  // optional macros
		nullptr,		                  // optional include files
		"GS_Main",		                  // entry point
		"gs_5_0",		                  // shader model (target)
		0,				                  // shader compile options
		0,				                  // effect compile options
		&gS,			                  // double pointer to ID3DBlob		
		nullptr
	);

	if (FAILED(hr)) {
		MessageBox(hostWndHandle, L"Geometry shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Compile Pixel Shader
	ID3DBlob* pS = nullptr;
	hr = D3DCompileFromFile(
		L"PixelShader.hlsl", // filename
		nullptr,		               // optional macros
		nullptr,		               // optional include files
		"PS_Main",		               // entry point
		"ps_5_0",		               // shader model (target)
		0,				               // shader compile options
		0,				               // effect compile options
		&pS,			               // double pointer to ID3DBlob		
		nullptr
	);

	if (FAILED(hr)) {
		MessageBox(hostWndHandle, L"Pixel shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create vertex-, geometry- and pixel shaders.
	if (FAILED(hr = gDevice->CreateVertexShader(vS->GetBufferPointer(), vS->GetBufferSize(), nullptr, &gVertexShader))) {
		MessageBox(hostWndHandle, L"Vertex shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	if (FAILED(hr = gDevice->CreateGeometryShader(gS->GetBufferPointer(), gS->GetBufferSize(), nullptr, &gGeometryShader))) {
		MessageBox(hostWndHandle, L"Geometry shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	if (FAILED(hr = gDevice->CreatePixelShader(pS->GetBufferPointer(), pS->GetBufferSize(), nullptr, &gPixelShader))) {
		MessageBox(hostWndHandle, L"Pixel shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create input layout.
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(hr = gDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vS->GetBufferPointer(), vS->GetBufferSize(), &gVertexLayout))) {
		MessageBox(hostWndHandle, L"Input layout could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Free the blobs (that sounds like a cute game).
	pS->Release();
	gS->Release();
	vS->Release();

	// Describe constant buffer.
	D3D11_BUFFER_DESC cbDesc;
	cbDesc.Usage               = D3D11_USAGE_DYNAMIC;
	cbDesc.ByteWidth           = sizeof(Matrices);
	cbDesc.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	cbDesc.MiscFlags           = 0;
	cbDesc.StructureByteStride = 0;

	// Create constant buffer
	if (FAILED(hr = gDevice->CreateBuffer(&cbDesc, nullptr, &gConstBuffer))) {
		MessageBox(hostWndHandle, L"Constant buffer could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create temp geometry.
	Vertex triangleVertices[4] =
	{
		{ -0.5f, -0.5f, 0.0f,
		0.0f, 1.0f, 0.5f },

		{ -0.5f, 0.5f, 0.0f,
		0.0f, 0.0f, 0.5f },

		{ 0.5f, -0.5f, 0.0f,
		1.0f, 1.0f, 0.5f },

		{ 0.5f, 0.5f, 0.0f,
		1.0f, 0.0f, 0.5f }
	};

	// Describe vertex buffer
	D3D11_BUFFER_DESC bDesc;
	ZeroMemory(&bDesc, sizeof(D3D11_BUFFER_DESC));

	bDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bDesc.Usage     = D3D11_USAGE_DEFAULT;
	bDesc.ByteWidth = sizeof(triangleVertices);

	// The data to tie to the vertex buffer.
	D3D11_SUBRESOURCE_DATA data;
	data.pSysMem = triangleVertices;

	// Create the vertex buffer.
	if(FAILED(hr = gDevice->CreateBuffer(&bDesc, &data, &gVertexBuffer))) {
		MessageBox(hostWndHandle, L"Vertex buffer could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	return hr;
}

HRESULT Rasterizer::render() {
	HRESULT hr;

	// Clear the buffers.
	float clearColor[] = { 0, 0, 0, 1 };
	gDeviceContext->ClearRenderTargetView(gRenderTargetView, clearColor);
	gDeviceContext->ClearDepthStencilView(gDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set shaders.
	gDeviceContext->VSSetShader(gVertexShader, nullptr, 0);
	gDeviceContext->HSSetShader(nullptr, nullptr, 0);
	gDeviceContext->DSSetShader(nullptr, nullptr, 0);
	gDeviceContext->GSSetShader(gGeometryShader, nullptr, 0);
	gDeviceContext->PSSetShader(gPixelShader, nullptr, 0);
	
	// Map constant buffer.
	D3D11_MAPPED_SUBRESOURCE data;
	if (FAILED(hr = gDeviceContext->Map(gConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data))) {
		MessageBox(hostWndHandle, L"Constant buffer could not be mapped.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Write to the constant buffer.
	memcpy(data.pData, &m, sizeof(Matrices));

	// Unmap the constant buffer.
	gDeviceContext->Unmap(gConstBuffer, 0);

	// Send matrices to geometry shader.
	gDeviceContext->GSSetConstantBuffers(0, 1, &gConstBuffer);

	// Render
	UINT32 vertexSize = sizeof(Vertex);
	UINT32 offset = 0;
	gDeviceContext->IASetVertexBuffers(0, 1, &gVertexBuffer, &vertexSize, &offset);

	gDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	gDeviceContext->IASetInputLayout(gVertexLayout);

	gDeviceContext->Draw(4, 0);

	if (FAILED(hr = gSwapChain->Present(0, 0))) {
		MessageBox(hostWndHandle, L"Swap chain failed to switch buffers.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	return hr;
}