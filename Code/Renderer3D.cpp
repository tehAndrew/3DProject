#include "Renderer3D.h"

Renderer3D::Renderer3D() {
	mDevice             = nullptr;
	mDeviceContext      = nullptr;
	mSwapChain          = nullptr;
	mRenderTargetView   = nullptr;
	mDepthStencilView   = nullptr;
	mDepthStencilBuffer = nullptr;
	mMatrixBuffer       = nullptr;
	mLightBuffer        = nullptr;
	mCameraBuffer       = nullptr;
	mVertexLayout       = nullptr;
	mVertexShader       = nullptr;
	mGeometryShader     = nullptr;
	mPixelShader        = nullptr;

	mCamera         = new FirstPersonCamera();
	mModels         = new std::vector<Model*>();
	mTextureManager = new TextureManager();
}

Renderer3D::~Renderer3D() {
	if (mDevice != nullptr)
		mDevice->Release();

	if (mDeviceContext != nullptr)
		mDeviceContext->Release();

	if (mSwapChain != nullptr)
		mSwapChain->Release();

	if (mRenderTargetView != nullptr)
		mRenderTargetView->Release();

	if (mDepthStencilView != nullptr)
		mDepthStencilView->Release();

	if (mDepthStencilBuffer != nullptr)
		mDepthStencilBuffer->Release();

	if (mMatrixBuffer != nullptr)
		mMatrixBuffer->Release();

	if (mLightBuffer != nullptr)
		mLightBuffer->Release();

	if (mCameraBuffer != nullptr)
		mCameraBuffer->Release();

	if (mVertexLayout != nullptr)
		mVertexLayout->Release();

	if (mVertexShader != nullptr)
		mVertexShader->Release();

	if (mGeometryShader != nullptr)
		mGeometryShader->Release();

	if (mPixelShader != nullptr)
		mPixelShader->Release();

	delete mTextureManager;

	for (auto itr = mModels->begin(); itr != mModels->end(); ++itr)
		delete *itr;

	delete mModels;
}

FirstPersonCamera* Renderer3D::getCamera() const {
	return mCamera;
}

void Renderer3D::createModelFromFile(wchar_t* filename, FXMVECTOR pos, FXMVECTOR rot, FXMVECTOR scale, Texture* texture) {
	Model* model = ModelCreator::loadModel(filename, pos, rot, scale, mDevice, texture);
	mModels->push_back(model);
}

void Renderer3D::createFloor(FXMVECTOR pos, FXMVECTOR rot, float side, Texture* texture) {
	Model* model = ModelCreator::generateFloor(pos, rot, side, mDevice, texture);
	mModels->push_back(model);
}

void Renderer3D::createCube(FXMVECTOR pos, FXMVECTOR rot, float side, Texture* texture) {
	Model* model = ModelCreator::generateCube(pos, rot, side, mDevice, texture);
	mModels->push_back(model);
}

void Renderer3D::createTexture(wchar_t* filename, std::string name) {
	mTextureManager->addTexture(mDevice, mDeviceContext, filename, name);
}

Texture* Renderer3D::getTexture(std::string name) const {
	return mTextureManager->getTexture(name);
}

HRESULT Renderer3D::initDirect3D(HWND hWnd, int clientWidth, int clientHeight) {
	HRESULT hr;

	// Set window handle
	mHostWndHandle = hWnd;

	// Describe the swapchain.
	DXGI_SWAP_CHAIN_DESC scDesc;
	ZeroMemory(&scDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	scDesc.BufferCount = 1;                                    // one back buffer
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;     // use 32-bit color
	scDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;      // how swap chain is to be used
	scDesc.OutputWindow = mHostWndHandle;                             // the window to be used
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
		&mSwapChain,
		&mDevice,
		NULL,
		&mDeviceContext
	);

	if (FAILED(hr)) {
		MessageBox(mHostWndHandle, L"Device context and swap chain could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
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
	if (FAILED(hr = mDevice->CreateTexture2D(&tDesc, 0, &mDepthStencilBuffer))) {
		MessageBox(mHostWndHandle, L"Depth/stencil buffer could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create the depth/stencil view.
	if (FAILED(hr = mDevice->CreateDepthStencilView(mDepthStencilBuffer, 0, &mDepthStencilView))) {
		MessageBox(mHostWndHandle, L"Depth/stencil view could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Get the address of the back buffer.
	ID3D11Texture2D* backBuffer = nullptr;
	if (FAILED(hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer))) {
		MessageBox(mHostWndHandle, L"Back buffer could not be fetched.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	// Create the render target view.
	if (FAILED(hr = mDevice->CreateRenderTargetView(backBuffer, NULL, &mRenderTargetView))) {
		MessageBox(mHostWndHandle, L"Back buffer could not be fetched.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}
	// We don't need the texture for the backbuffer anymore.
	backBuffer->Release();

	// Set render target view and depth/stencil view.
	mDeviceContext->OMSetRenderTargets(1, &mRenderTargetView, mDepthStencilView);

	// Describe view port.
	D3D11_VIEWPORT viewPort;
	viewPort.Width    = (float) clientWidth;
	viewPort.Height   = (float) clientHeight;
	viewPort.MinDepth = 0.0f;
	viewPort.MaxDepth = 1.0f;
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;

	// Set view port.
	mDeviceContext->RSSetViewports(1, &viewPort);

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
		MessageBox(mHostWndHandle, L"Vertex shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
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
		MessageBox(mHostWndHandle, L"Geometry shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
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
		MessageBox(mHostWndHandle, L"Pixel shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create vertex-, geometry- and pixel shaders.
	if (FAILED(hr = mDevice->CreateVertexShader(vS->GetBufferPointer(), vS->GetBufferSize(), nullptr, &mVertexShader))) {
		MessageBox(mHostWndHandle, L"Vertex shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	if (FAILED(hr = mDevice->CreateGeometryShader(gS->GetBufferPointer(), gS->GetBufferSize(), nullptr, &mGeometryShader))) {
		MessageBox(mHostWndHandle, L"Geometry shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	if (FAILED(hr = mDevice->CreatePixelShader(pS->GetBufferPointer(), pS->GetBufferSize(), nullptr, &mPixelShader))) {
		MessageBox(mHostWndHandle, L"Pixel shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create input layout.
	D3D11_INPUT_ELEMENT_DESC inputDesc[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12 , D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 24 , D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};

	if (FAILED(hr = mDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vS->GetBufferPointer(), vS->GetBufferSize(), &mVertexLayout))) {
		MessageBox(mHostWndHandle, L"Input layout could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Free the blobs (that sounds like a cute game).
	pS->Release();
	gS->Release();
	vS->Release();

	// Describe constant buffer.
	D3D11_BUFFER_DESC cbDesc1;
	cbDesc1.Usage               = D3D11_USAGE_DYNAMIC;
	cbDesc1.ByteWidth           = sizeof(MatrixData);
	cbDesc1.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	cbDesc1.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	cbDesc1.MiscFlags           = 0;
	cbDesc1.StructureByteStride = 0;

	// Create constant buffer
	if (FAILED(hr = mDevice->CreateBuffer(&cbDesc1, nullptr, &mMatrixBuffer))) {
		MessageBox(mHostWndHandle, L"Constant buffer could not be created1.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Describe constant buffer.
	D3D11_BUFFER_DESC cbDesc2;
	cbDesc2.Usage			    = D3D11_USAGE_DYNAMIC;
	cbDesc2.ByteWidth			= sizeof(LightData);
	cbDesc2.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cbDesc2.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	cbDesc2.MiscFlags           = 0;
	cbDesc2.StructureByteStride = 0;

	// Create constant buffer
	if (FAILED(hr = mDevice->CreateBuffer(&cbDesc2, nullptr, &mLightBuffer))) {
		MessageBox(mHostWndHandle, L"Constant buffer could not be created2.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Describe constant buffer.
	D3D11_BUFFER_DESC cbDesc3;
	cbDesc3.Usage				= D3D11_USAGE_DYNAMIC;
	cbDesc3.ByteWidth			= sizeof(CameraData);
	cbDesc3.BindFlags			= D3D11_BIND_CONSTANT_BUFFER;
	cbDesc3.CPUAccessFlags		= D3D11_CPU_ACCESS_WRITE;
	cbDesc3.MiscFlags			= 0;
	cbDesc3.StructureByteStride = 0;

	// Create constant buffer
	if (FAILED(hr = mDevice->CreateBuffer(&cbDesc3, nullptr, &mCameraBuffer))) {
		MessageBox(mHostWndHandle, L"Constant buffer could not be created3.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	mTextureManager->initTextureManager(mDevice);

	return hr;
}

HRESULT Renderer3D::render() {
	HRESULT hr;

	// Clear the buffers.
	float clearColor[] = { 0, 0, 0, 1 };
	mDeviceContext->ClearRenderTargetView(mRenderTargetView, clearColor);
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Set shaders.
	mDeviceContext->VSSetShader(mVertexShader, nullptr, 0);
	mDeviceContext->HSSetShader(nullptr, nullptr, 0);
	mDeviceContext->DSSetShader(nullptr, nullptr, 0);
	mDeviceContext->GSSetShader(mGeometryShader, nullptr, 0);
	mDeviceContext->PSSetShader(mPixelShader, nullptr, 0);
	// TEMP
	ID3D11SamplerState* ssTemp = mTextureManager->getSamplerState();
	mDeviceContext->PSSetSamplers(0, 1, &ssTemp);

	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	mDeviceContext->IASetInputLayout(mVertexLayout);

	// Update view  and projection matrices
	mMatrixData.viewMatrix = mCamera->getViewMatrix();
	mMatrixData.projMatrix = mCamera->getProjMatrix();

	// Update Light data
	mLightData.ambientColor  = {0.1f, 0.1f, 0.1f, 1.f};
	mLightData.diffuseColor  = {1.f, 1.f, 1.f, 1.f};
	mLightData.specularColor = {0.5f, 0.5f, 0.5f, 1.f};
	mLightData.lightPos      = {0.f, 1000.f, 0.f, 1.f};
	mLightData.specPow       = 64.f;

	// Update camera pos
	XMStoreFloat4(&mCameraData.cameraPos, mCamera->getPos());

	for (auto it = mModels->begin(); it != mModels->end(); ++it) {
		// Get world matrix of current model.
		mMatrixData.worldMatrix = (*it)->getWorldMatrix();

		// Map constant buffer.
		D3D11_MAPPED_SUBRESOURCE data;
		if (FAILED(hr = mDeviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data))) {
			MessageBox(mHostWndHandle, L"Constant buffer could not be mapped.", L"D3D Error!", MB_OK | MB_ICONERROR);
			return hr;
		}
		memcpy(data.pData, &mMatrixData, sizeof(MatrixData));
		mDeviceContext->Unmap(mMatrixBuffer, 0);

		// Map constant buffer.
		//D3D11_MAPPED_SUBRESOURCE data;
		if (FAILED(hr = mDeviceContext->Map(mLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data))) {
			MessageBox(mHostWndHandle, L"Constant buffer could not be mapped.", L"D3D Error!", MB_OK | MB_ICONERROR);
			return hr;
		}
		memcpy(data.pData, &mLightData, sizeof(LightData));
		mDeviceContext->Unmap(mLightBuffer, 0);

		// Map constant buffer.
		//D3D11_MAPPED_SUBRESOURCE data;
		if (FAILED(hr = mDeviceContext->Map(mCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data))) {
			MessageBox(mHostWndHandle, L"Constant buffer could not be mapped.", L"D3D Error!", MB_OK | MB_ICONERROR);
			return hr;
		}
		memcpy(data.pData, &mCameraData, sizeof(CameraData));
		mDeviceContext->Unmap(mCameraBuffer, 0);

		// Send matrices to geometry shader.
		mDeviceContext->GSSetConstantBuffers(0, 1, &mMatrixBuffer);

		mDeviceContext->PSSetConstantBuffers(0, 1, &mLightBuffer);
		mDeviceContext->PSSetConstantBuffers(1, 1, &mCameraBuffer);

		(*it)->render(mDeviceContext);
	}
	
	if (FAILED(hr = mSwapChain->Present(0, 0))) {
		MessageBox(mHostWndHandle, L"Swap chain failed to switch buffers.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	return hr;
}