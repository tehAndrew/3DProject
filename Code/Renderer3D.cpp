#include "Renderer3D.h"

Renderer3D::Renderer3D() {
	mDevice        = nullptr;
	mDeviceContext = nullptr;
	mSwapChain     = nullptr;
	mMatrixBuffer  = nullptr;
	mLightBuffer   = nullptr;
	mCameraBuffer  = nullptr;
	mFrustumBuffer = nullptr;

	mSamplerState       = nullptr;
	mShadowSamplerState = nullptr;

	for (int i = 0; i < BUFFER_AMOUNT; i++) {
		mRenderBuffers[i]       = nullptr;
		mRenderTargetViews[i]   = nullptr;
		mShaderResourceViews[i] = nullptr;
	}
	mInputLayoutGp      = nullptr;
	mVertexShaderGp     = nullptr;
	mGeometryShaderGp   = nullptr;
	mPixelShaderGp      = nullptr;
	mDepthStencilView   = nullptr;
	mDepthStencilBuffer = nullptr;

	mInputLayoutSp  = nullptr;
	mVertexShaderSp = nullptr;
	mPixelShaderSp  = nullptr;

	mBackBufferRtv  = nullptr;
	mInputLayoutLp  = nullptr;
	mVertexShaderLp = nullptr;
	mPixelShaderLp  = nullptr;
	mVertexBufferLp = nullptr;

	mCamera         = new FirstPersonCamera();
	mPointLight     = new PointLight();
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

	if (mMatrixBuffer != nullptr)
		mMatrixBuffer->Release();

	if (mLightBuffer != nullptr)
		mLightBuffer->Release();

	if (mCameraBuffer != nullptr)
		mCameraBuffer->Release();

	if (mFrustumBuffer != nullptr)
		mFrustumBuffer->Release();

	if (mSamplerState != nullptr)
		mSamplerState->Release();

	if (mShadowSamplerState != nullptr)
		mShadowSamplerState->Release();

	for (int i = 0; i < BUFFER_AMOUNT; i++) {
		if (mRenderBuffers[i] != nullptr)
			mRenderBuffers[i]->Release();

		if (mRenderTargetViews[i] != nullptr)
			mRenderTargetViews[i]->Release();

		if (mShaderResourceViews[i] != nullptr)
			mShaderResourceViews[i]->Release();
	}

	if (mInputLayoutGp != nullptr)
		mInputLayoutGp->Release();

	if (mVertexShaderGp != nullptr)
		mVertexShaderGp->Release();

	if (mGeometryShaderGp != nullptr)
		mGeometryShaderGp->Release();

	if (mPixelShaderGp != nullptr)
		mPixelShaderGp->Release();

	if (mDepthStencilView != nullptr)
		mDepthStencilView->Release();

	if (mDepthStencilBuffer != nullptr)
		mDepthStencilBuffer->Release();

	if (mInputLayoutSp != nullptr)
		mInputLayoutSp->Release();

	if (mVertexShaderSp != nullptr)
		mVertexShaderSp->Release();

	if (mPixelShaderSp != nullptr)
		mPixelShaderSp->Release();

	if (mBackBufferRtv != nullptr)
		mBackBufferRtv->Release();

	if (mInputLayoutLp != nullptr)
		mInputLayoutLp->Release();

	if (mVertexShaderLp != nullptr)
		mVertexShaderLp->Release();

	if (mPixelShaderLp != nullptr)
		mPixelShaderLp->Release();

	if (mVertexBufferLp != nullptr)
		mVertexBufferLp->Release();

	delete mCamera;
	delete mPointLight;
	delete mTextureManager;

	for (auto itr = mModels->begin(); itr != mModels->end(); ++itr)
		delete *itr;

	delete mModels;
}

HRESULT Renderer3D::initGeometryShaderPass(int clientWidth, int clientHeight) {
	HRESULT hr;
	ID3DBlob* vsBlob;
	ID3DBlob* gsBlob;
	ID3DBlob* psBlob;
	D3D11_TEXTURE2D_DESC textureDesc;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_INPUT_ELEMENT_DESC inputDesc[3];
	D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	
	// Descrition

	// Compile Vertex Shader
	vsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"gpVertexShader.hlsl",
		nullptr,
		nullptr,
		"VS_Main",
		"vs_5_0",
		0,
		0,
		&vsBlob,		
		nullptr
	);

	if (FAILED(hr)) {
		MessageBox(mHostWndHandle, L"Vertex shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Compile Geometry Shader
	gsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"gpGeometryShader.hlsl",
		nullptr,	
		nullptr,	
		"GS_Main",		
		"gs_5_0",	
		0,
		0,				                
		&gsBlob,	
		nullptr
	);

	if (FAILED(hr)) {
		MessageBox(mHostWndHandle, L"Geometry shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Compile Pixel Shader
	psBlob = nullptr;
	hr = D3DCompileFromFile(
		L"gpPixelShader.hlsl",
		nullptr,
		nullptr,
		"PS_Main",
		"ps_5_0",
		0,
		0,
		&psBlob,	
		nullptr
	);

	if (FAILED(hr)) {
		MessageBox(mHostWndHandle, L"Pixel shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create vertex-, geometry- and pixel shaders.
	if (FAILED(hr = mDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &mVertexShaderGp))) {
		MessageBox(mHostWndHandle, L"Vertex shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	if (FAILED(hr = mDevice->CreateGeometryShader(gsBlob->GetBufferPointer(), gsBlob->GetBufferSize(), nullptr, &mGeometryShaderGp))) {
		MessageBox(mHostWndHandle, L"Geometry shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	if (FAILED(hr = mDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &mPixelShaderGp))) {
		MessageBox(mHostWndHandle, L"Pixel shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create input layout.
	inputDesc[0].SemanticName		  = "POSITION";
	inputDesc[0].SemanticIndex		  = 0;
	inputDesc[0].Format				  = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[0].InputSlot			  = 0;
	inputDesc[0].AlignedByteOffset    = 0;
	inputDesc[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[0].InstanceDataStepRate = 0;

	inputDesc[1].SemanticName		  = "NORMAL";
	inputDesc[1].SemanticIndex		  = 0;
	inputDesc[1].Format				  = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[1].InputSlot			  = 0;
	inputDesc[1].AlignedByteOffset	  = 12;
	inputDesc[1].InputSlotClass		  = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[1].InstanceDataStepRate = 0;
	
	inputDesc[2].SemanticName		  = "TEXCOORD";
	inputDesc[2].SemanticIndex		  = 0;
	inputDesc[2].Format			      = DXGI_FORMAT_R32G32_FLOAT;
	inputDesc[2].InputSlot            = 0;
	inputDesc[2].AlignedByteOffset	  = 24;
	inputDesc[2].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[2].InstanceDataStepRate = 0;

	if (FAILED(hr = mDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayoutGp))) {
		MessageBox(mHostWndHandle, L"Input layout could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Free the blobs (that sounds like a cute game).
	psBlob->Release();
	gsBlob->Release();
	vsBlob->Release();

	// Describe textures
	ZeroMemory(&textureDesc, sizeof(textureDesc));
	textureDesc.Width			 = clientWidth;
	textureDesc.Height			 = clientHeight;
	textureDesc.MipLevels		 = 1;
	textureDesc.ArraySize		 = 1;
	textureDesc.Format			 = DXGI_FORMAT_R32G32B32A32_FLOAT;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.Usage			 = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags		 = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	// Create texture buffers.
	for (int i = 0; i < BUFFER_AMOUNT; i++)
		if (FAILED(hr = mDevice->CreateTexture2D(&textureDesc, NULL, &mRenderBuffers[i])))
			return hr;

	// Describe render target view.
	rtvDesc.Format			   = textureDesc.Format;
	rtvDesc.ViewDimension	   = D3D11_RTV_DIMENSION_TEXTURE2D;
	rtvDesc.Texture2D.MipSlice = 0;

	for (int i = 0; i < BUFFER_AMOUNT; i++)
		if (FAILED(hr = mDevice->CreateRenderTargetView(mRenderBuffers[i], &rtvDesc, &mRenderTargetViews[i])))
			return hr;

	// Describe shader resource view
	srvDesc.Format                    = textureDesc.Format;
	srvDesc.ViewDimension             = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvDesc.Texture2D.MostDetailedMip = 0;
	srvDesc.Texture2D.MipLevels       = 1;

	for (int i = 0; i < BUFFER_AMOUNT; i++)
		if (FAILED(hr = mDevice->CreateShaderResourceView(mRenderBuffers[i], &srvDesc, &mShaderResourceViews[i])))
			return hr;

	// Describe the depth buffer
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width			 = clientWidth;
	depthBufferDesc.Height			 = clientHeight;
	depthBufferDesc.MipLevels		 = 1;
	depthBufferDesc.ArraySize		 = 1;
	depthBufferDesc.Format			 = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.Usage			 = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags		 = D3D11_BIND_DEPTH_STENCIL;

	if (FAILED(hr = mDevice->CreateTexture2D(&depthBufferDesc, NULL, &mDepthStencilBuffer)))
		return hr;

	ZeroMemory(&dsvDesc, sizeof(dsvDesc));
	dsvDesc.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	dsvDesc.ViewDimension      = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;

	if (FAILED(hr = mDevice->CreateDepthStencilView(mDepthStencilBuffer, &dsvDesc, &mDepthStencilView)))
		return hr;

	return hr;
}

HRESULT Renderer3D::initShadowMapShaderPass() {
	HRESULT hr;
	D3D11_INPUT_ELEMENT_DESC inputDesc[3];
	ID3DBlob* vsBlob;
	ID3DBlob* psBlob;

	// Compile Vertex Shader
	vsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"spVertexShader.hlsl",
		nullptr,
		nullptr,
		"VS_Main",
		"vs_5_0",
		0,
		0,
		&vsBlob,
		nullptr
	);

	if (FAILED(hr)) {
		MessageBox(mHostWndHandle, L"Vertex shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Compile Pixel Shader
	psBlob = nullptr;
	hr = D3DCompileFromFile(
		L"spEmptyPixelShader.hlsl",
		nullptr,
		nullptr,
		"PS_Main",
		"ps_5_0",
		0,
		0,
		&psBlob,
		nullptr
	);

	if (FAILED(hr)) {
		MessageBox(mHostWndHandle, L"Pixel shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create vertex- and pixel shaders.
	if (FAILED(hr = mDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &mVertexShaderSp))) {
		MessageBox(mHostWndHandle, L"Vertex shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	if (FAILED(hr = mDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &mPixelShaderSp))) {
		MessageBox(mHostWndHandle, L"Vertex shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create input layout.
	inputDesc[0].SemanticName = "POSITION";
	inputDesc[0].SemanticIndex = 0;
	inputDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[0].InputSlot = 0;
	inputDesc[0].AlignedByteOffset = 0;
	inputDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[0].InstanceDataStepRate = 0;

	inputDesc[1].SemanticName = "NORMAL";
	inputDesc[1].SemanticIndex = 0;
	inputDesc[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[1].InputSlot = 0;
	inputDesc[1].AlignedByteOffset = 12;
	inputDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[1].InstanceDataStepRate = 0;

	inputDesc[2].SemanticName = "TEXCOORD";
	inputDesc[2].SemanticIndex = 0;
	inputDesc[2].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputDesc[2].InputSlot = 0;
	inputDesc[2].AlignedByteOffset = 24;
	inputDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[2].InstanceDataStepRate = 0;

	if (FAILED(hr = mDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayoutSp))) {
		MessageBox(mHostWndHandle, L"Input layout could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	vsBlob->Release();
	psBlob->Release();

	return hr;
}

HRESULT Renderer3D::initLightShaderPass(int clientWidth, int clientHeight) {
	HRESULT hr;
	ID3DBlob* vsBlob;
	ID3DBlob* psBlob;
	D3D11_INPUT_ELEMENT_DESC inputDesc[2];
	D3D11_BUFFER_DESC screenQuadDesc;
	D3D11_SUBRESOURCE_DATA vertexData;
	ID3D11Texture2D* backBuffer;

	// Compile Vertex Shader
	vsBlob = nullptr;
	hr = D3DCompileFromFile(
		L"lpVertexShader.hlsl",
		nullptr,
		nullptr,
		"VS_Main",
		"vs_5_0",
		0,
		0,
		&vsBlob,
		nullptr
	);

	if (FAILED(hr)) {
		MessageBox(mHostWndHandle, L"Vertex shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Compile Pixel Shader
	psBlob = nullptr;
	hr = D3DCompileFromFile(
		L"lpPixelShader.hlsl",
		nullptr,
		nullptr,
		"PS_Main",
		"ps_5_0",
		0,
		0,
		&psBlob,
		nullptr
	);

	if (FAILED(hr)) {
		MessageBox(mHostWndHandle, L"Pixel shader could not be compiled.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create vertex-, geometry- and pixel shaders.
	if (FAILED(hr = mDevice->CreateVertexShader(vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), nullptr, &mVertexShaderLp))) {
		MessageBox(mHostWndHandle, L"Vertex shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	if (FAILED(hr = mDevice->CreatePixelShader(psBlob->GetBufferPointer(), psBlob->GetBufferSize(), nullptr, &mPixelShaderLp))) {
		MessageBox(mHostWndHandle, L"Pixel shader could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	inputDesc[0].SemanticName         = "POSITION";
	inputDesc[0].SemanticIndex        = 0;
	inputDesc[0].Format               = DXGI_FORMAT_R32G32B32_FLOAT;
	inputDesc[0].InputSlot            = 0;
	inputDesc[0].AlignedByteOffset    = 0;
	inputDesc[0].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[0].InstanceDataStepRate = 0;

	inputDesc[1].SemanticName         = "TEXCOORD";
	inputDesc[1].SemanticIndex        = 0;
	inputDesc[1].Format               = DXGI_FORMAT_R32G32_FLOAT;
	inputDesc[1].InputSlot            = 0;
	inputDesc[1].AlignedByteOffset    = 12;
	inputDesc[1].InputSlotClass       = D3D11_INPUT_PER_VERTEX_DATA;
	inputDesc[1].InstanceDataStepRate = 0;

	if (FAILED(hr = mDevice->CreateInputLayout(inputDesc, ARRAYSIZE(inputDesc), vsBlob->GetBufferPointer(), vsBlob->GetBufferSize(), &mInputLayoutLp))) {
		MessageBox(mHostWndHandle, L"Input layout could not be created.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	vsBlob->Release();
	psBlob->Release();

	// Get the address of the back buffer.
	backBuffer = nullptr;
	if (FAILED(hr = mSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer))) {
		MessageBox(mHostWndHandle, L"Back buffer could not be fetched.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Create the render target view.
	if (FAILED(hr = mDevice->CreateRenderTargetView(backBuffer, NULL, &mBackBufferRtv))) {
		MessageBox(mHostWndHandle, L"Back buffer could not be fetched.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}
	// We don't need the texture for the backbuffer anymore.
	backBuffer->Release();

	// Define screen quad
	VertexLp screenQuad[] = { 
		{-1.f, -1.f, 0.f, 0.f, 1.f},
		{-1.f, 1.f, 0.f, 0.f, 0.f},
		{1.f, 1.f, 0.f, 1.f, 0.f},
		{-1.f, -1.f, 0.f, 0.f, 1.f},
		{1.f, 1.f, 0.f, 1.f, 0.f},
		{1.f, -1.f, 0.f, 1.f, 1.f} 
	};

	screenQuadDesc.Usage               = D3D11_USAGE_DEFAULT;
	screenQuadDesc.ByteWidth           = sizeof(VertexLp) * 6;
	screenQuadDesc.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	screenQuadDesc.CPUAccessFlags      = 0;
	screenQuadDesc.MiscFlags           = 0;
	screenQuadDesc.StructureByteStride = 0;

	vertexData.pSysMem          = screenQuad;
	vertexData.SysMemPitch      = 0;
	vertexData.SysMemSlicePitch = 0;

	if (FAILED(hr = mDevice->CreateBuffer(&screenQuadDesc, &vertexData, &mVertexBufferLp))) {
		MessageBox(mHostWndHandle, L"Back buffer could not be fetched.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	return hr;
}

HRESULT Renderer3D::initDirect3D(HWND hWnd, int clientWidth, int clientHeight, float nearPlane, float farPlane) {
	HRESULT hr;
	DXGI_SWAP_CHAIN_DESC scDesc;
	D3D11_BUFFER_DESC cbMatrix, cbLight, cbCamera, cbFrustum;
	D3D11_SAMPLER_DESC samplerDesc, shadowSamplerDesc;

	// Set window handle
	mHostWndHandle = hWnd;

	mClientWidth  = clientWidth;
	mClientHeight = clientHeight;
	mNearPlane    = nearPlane;
	mFarPlane     = farPlane;

	// Describe the swapchain.
	ZeroMemory(&scDesc, sizeof(scDesc));
	scDesc.BufferCount       = 1;
	scDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scDesc.BufferUsage       = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scDesc.OutputWindow      = mHostWndHandle; 
	scDesc.SampleDesc.Count  = 1;
	scDesc.Windowed          = true;

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

	// Describe view port.
	mSceneViewPort.Width = (float)clientWidth;
	mSceneViewPort.Height = (float)clientHeight;
	mSceneViewPort.MinDepth = 0.0f;
	mSceneViewPort.MaxDepth = 1.0f;
	mSceneViewPort.TopLeftX = 0;
	mSceneViewPort.TopLeftY = 0;

	// Describe constant buffer.
	cbMatrix.Usage               = D3D11_USAGE_DYNAMIC;
	cbMatrix.ByteWidth           = sizeof(MatrixData);
	cbMatrix.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	cbMatrix.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	cbMatrix.MiscFlags           = 0;
	cbMatrix.StructureByteStride = 0;

	// Create constant buffer
	if (FAILED(hr = mDevice->CreateBuffer(&cbMatrix, nullptr, &mMatrixBuffer))) {
		MessageBox(mHostWndHandle, L"Constant buffer could not be created1.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Describe constant buffer.
	cbLight.Usage               = D3D11_USAGE_DYNAMIC;
	cbLight.ByteWidth           = sizeof(LightData);
	cbLight.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	cbLight.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	cbLight.MiscFlags           = 0;
	cbLight.StructureByteStride = 0;

	// Create constant buffer
	if (FAILED(hr = mDevice->CreateBuffer(&cbLight, nullptr, &mLightBuffer))) {
		MessageBox(mHostWndHandle, L"Constant buffer could not be created2.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	// Describe constant buffer.
	cbCamera.Usage               = D3D11_USAGE_DYNAMIC;
	cbCamera.ByteWidth           = sizeof(CameraData);
	cbCamera.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	cbCamera.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	cbCamera.MiscFlags           = 0;
	cbCamera.StructureByteStride = 0;

	// Create constant buffer
	if (FAILED(hr = mDevice->CreateBuffer(&cbCamera, nullptr, &mCameraBuffer))) {
		MessageBox(mHostWndHandle, L"Constant buffer could not be created3.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}
	
	// Describe constant buffer.
	cbFrustum.Usage               = D3D11_USAGE_DYNAMIC;
	cbFrustum.ByteWidth           = sizeof(FrustumData);
	cbFrustum.BindFlags           = D3D11_BIND_CONSTANT_BUFFER;
	cbFrustum.CPUAccessFlags      = D3D11_CPU_ACCESS_WRITE;
	cbFrustum.MiscFlags           = 0;
	cbFrustum.StructureByteStride = 0;

	// Create constant buffer
	if (FAILED(hr = mDevice->CreateBuffer(&cbFrustum, nullptr, &mFrustumBuffer))) {
		MessageBox(mHostWndHandle, L"Constant buffer could not be created4.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

    // Give paddings a value
	mFrustumData.Pad1 = 0.f;
	mFrustumData.Pad2 = 0.f;
	mFrustumData.Pad3 = 0.f;

	// Create sampler states
	ZeroMemory(&shadowSamplerDesc, sizeof(shadowSamplerDesc));

	shadowSamplerDesc.Filter         = D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
	shadowSamplerDesc.AddressU       = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressV       = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.AddressW       = D3D11_TEXTURE_ADDRESS_BORDER;
	shadowSamplerDesc.BorderColor[0] = 0.f;
	shadowSamplerDesc.BorderColor[1] = 0.f;
	shadowSamplerDesc.BorderColor[2] = 0.f;
	shadowSamplerDesc.BorderColor[3] = 0.f;
	shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

	if (FAILED(hr = mDevice->CreateSamplerState(&shadowSamplerDesc, &mShadowSamplerState)))
		return hr;

	initGeometryShaderPass(clientWidth, clientHeight);
	initShadowMapShaderPass();
	initLightShaderPass(clientWidth, clientHeight);
	mPointLight->initPointLight(mDevice, mNearPlane, mFarPlane);

	return hr;
}

FirstPersonCamera* Renderer3D::getCamera() const {
	return mCamera;
}

PointLight* Renderer3D::getPointLight() const {
	return mPointLight;
}

std::vector<Model*>* Renderer3D::getModelArray() const {
	return mModels;
}

void Renderer3D::createModelFromFile(wchar_t* filename, FXMVECTOR pos, FXMVECTOR rot, FXMVECTOR scale) {
	Model* model = ModelCreator::loadModel(filename, pos, rot, scale, mDevice, mDeviceContext, mTextureManager);
	mModels->push_back(model);
}

void Renderer3D::createFloor(FXMVECTOR pos, FXMVECTOR rot, float side, float detail, FXMVECTOR reflectiveColor, float gloss, Texture* texture) {
	Model* model = ModelCreator::generateFloor(pos, rot, side, mDevice, detail, reflectiveColor, gloss, texture);
	mModels->push_back(model);
}

//void Renderer3D::createCube(FXMVECTOR pos, FXMVECTOR rot, float side, Texture* texture) {
//	Model* model = ModelCreator::generateCube(pos, rot, side, mDevice, texture);
//	mModels->push_back(model);
//}

Texture* Renderer3D::getTexture(wchar_t* filename) {
	
	if (!mTextureManager->textureAlreadyLoaded(filename))
		mTextureManager->addTexture(mDevice, mDeviceContext, filename); 

	return mTextureManager->getTexture(filename);
}

HRESULT Renderer3D::render() {
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE data;

	// Clear buffers
	float clearColor[] = { 0, 0, 0, 1 };
	mDeviceContext->ClearRenderTargetView(mBackBufferRtv, clearColor);
	for (int i = 0; i < BUFFER_AMOUNT; i++)
		mDeviceContext->ClearRenderTargetView(mRenderTargetViews[i], clearColor);
	
	mDeviceContext->ClearDepthStencilView(mDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	for (int i = 0; i < 6; i++)
		mDeviceContext->ClearDepthStencilView(mPointLight->getShadowMapDsv(i), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);

	// Map light data
	mLightData.ambientColor = mPointLight->getAmbientColor();
	mLightData.diffuseColor = mPointLight->getDiffuseColor();
	mLightData.lightPos     = mPointLight->getPos();
	mLightData.attenuation  = mPointLight->getAttenuation();

	if (FAILED(hr = mDeviceContext->Map(mLightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data))) {
		MessageBox(mHostWndHandle, L"Constant buffer could not be mapped.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}
	memcpy(data.pData, &mLightData, sizeof(LightData));
	mDeviceContext->Unmap(mLightBuffer, 0);

	// Map frustum data
	mFrustumData.FarPlane = mFarPlane;

	if (FAILED(hr = mDeviceContext->Map(mFrustumBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data))) {
		MessageBox(mHostWndHandle, L"Constant buffer could not be mapped.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}
	memcpy(data.pData, &mFrustumData, sizeof(FrustumData));
	mDeviceContext->Unmap(mFrustumBuffer, 0);

	// Render
	if (FAILED(hr = renderGeometryPass()))
		return hr;

	if (FAILED(hr = renderShadowMapPass()))
		return hr;

	if (FAILED(hr = renderLightPass()))
		return hr;

	// Present
	if (FAILED(hr = mSwapChain->Present(0, 0))) {
		MessageBox(mHostWndHandle, L"Swap chain failed to switch buffers.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}

	return hr;
}

HRESULT Renderer3D::renderGeometryPass() {
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE data;

	mDeviceContext->RSSetViewports(1, &mSceneViewPort);

	//---------------------------------------------------------------------------------------------
	// UPDATE BUFFER DATA SHADER GLOBAL
	//---------------------------------------------------------------------------------------------
	// Update view  and projection matrices
	mMatrixData.viewMatrix = mCamera->getViewMatrix();
	mMatrixData.projMatrix = mCamera->getProjMatrix();

	//---------------------------------------------------------------------------------------------
	// SHADER SETTINGS SHADER GLOBAL
	//---------------------------------------------------------------------------------------------
	// Set render target.
	mDeviceContext->OMSetRenderTargets(BUFFER_AMOUNT, mRenderTargetViews, mDepthStencilView);

	// Set shaders, input layout and primitive type.
	setShaders(mInputLayoutGp, mVertexShaderGp, nullptr, nullptr, mGeometryShaderGp, mPixelShaderGp);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set sampler state.
	mDeviceContext->PSSetSamplers(0, 1, &mSamplerState);


	for (auto it = mModels->begin(); it != mModels->end(); ++it) {
		//-----------------------------------------------------------------------------------------
		// MAP BUFFERS MESH LOCAL
		//-----------------------------------------------------------------------------------------
		mMatrixData.worldMatrix = (*it)->getWorldMatrix();

		if (FAILED(hr = mDeviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data))) {
			MessageBox(mHostWndHandle, L"Constant buffer could not be mapped.", L"D3D Error!", MB_OK | MB_ICONERROR);
			return hr;
		}
		memcpy(data.pData, &mMatrixData, sizeof(MatrixData));
		mDeviceContext->Unmap(mMatrixBuffer, 0);

		ID3D11Buffer* material = (*it)->getMaterial();

		// Set constant buffers and shader resources.
		ID3D11ShaderResourceView* diffuseTextureMap = (*it)->getTexture();
		ID3D11Buffer*             materialBuffer = (*it)->getMaterial();
		mDeviceContext->PSSetShaderResources(0, 1, &diffuseTextureMap);
		mDeviceContext->PSSetConstantBuffers(0, 1, &material);
		mDeviceContext->GSSetConstantBuffers(0, 1, &mMatrixBuffer);

		// Render model
		(*it)->render(mDeviceContext);
	}

	mDeviceContext->PSSetSamplers(0, 0, nullptr);
	mDeviceContext->PSSetShaderResources(0, 0, nullptr);
	mDeviceContext->PSSetConstantBuffers(0, 0, nullptr);
	mDeviceContext->GSSetConstantBuffers(0, 0, nullptr);

	return hr;
}

HRESULT Renderer3D::renderShadowMapPass() {
	HRESULT hr;

	mDeviceContext->RSSetViewports(1, &(mPointLight->getViewPort()));

	//---------------------------------------------------------------------------------------------
	// SHADER SETTINGS SHADER GLOBAL
	//---------------------------------------------------------------------------------------------
	// Set shaders, input layout and primitive type.
	setShaders(mInputLayoutSp, mVertexShaderSp, nullptr, nullptr, nullptr, mPixelShaderSp);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	for (int i = 0; i < 6; i++) {
		ID3D11RenderTargetView* renderTargets[1] = { 0 };
		mDeviceContext->OMSetRenderTargets(0, renderTargets, mPointLight->getShadowMapDsv(i));
		for (auto it = mModels->begin(); it != mModels->end(); ++it) {
			//-----------------------------------------------------------------------------------------
			// MAP BUFFERS MESH LOCAL
			//-----------------------------------------------------------------------------------------

			if (FAILED(hr = mPointLight->mapMatrices(mDeviceContext, (*it)->getWorldMatrix(), i)))
				return hr;

			// Set constant buffers and shader resources.
			ID3D11Buffer* matrixBuffer = mPointLight->getMatrixBuffer();
			mDeviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);
			mDeviceContext->PSSetConstantBuffers(0, 1, &mLightBuffer);
			mDeviceContext->PSSetConstantBuffers(1, 1, &mFrustumBuffer);

			// Render model
			(*it)->render(mDeviceContext);
		}
	}
	mDeviceContext->VSSetConstantBuffers(0, 0, nullptr);
	mDeviceContext->PSSetConstantBuffers(0, 0, nullptr);
	mDeviceContext->PSSetConstantBuffers(1, 0, nullptr);

	return hr;
}

HRESULT Renderer3D::renderLightPass() {
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE data;

	mDeviceContext->RSSetViewports(1, &mSceneViewPort);
	
	//---------------------------------------------------------------------------------------------
	// MAP BUFFERS
	//---------------------------------------------------------------------------------------------
	mCameraData.cameraPos = mCamera->getPos();

	// Camera buffer
	if (FAILED(hr = mDeviceContext->Map(mCameraBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data))) {
		MessageBox(mHostWndHandle, L"Constant buffer could not be mapped.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}
	memcpy(data.pData, &mCameraData, sizeof(CameraData));
	mDeviceContext->Unmap(mCameraBuffer, 0);

	if (FAILED(hr = mDeviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data))) {
		MessageBox(mHostWndHandle, L"Constant buffer could not be mapped.", L"D3D Error!", MB_OK | MB_ICONERROR);
		return hr;
	}
	memcpy(data.pData, &mMatrixData, sizeof(MatrixData));
	mDeviceContext->Unmap(mMatrixBuffer, 0);

	//---------------------------------------------------------------------------------------------
	// SET RENDER STATES
	//---------------------------------------------------------------------------------------------
	// Set render target.
	mDeviceContext->OMSetRenderTargets(1, &mBackBufferRtv, nullptr);

	// Set shaders, input layout and primitive type.
	setShaders(mInputLayoutLp, mVertexShaderLp, nullptr, nullptr, nullptr, mPixelShaderLp);
	mDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// Set sampler state.
	mDeviceContext->PSSetSamplers(0, 1, &mSamplerState);

	// Set textures.
	for (int i = 0; i < BUFFER_AMOUNT; i++)
		mDeviceContext->PSSetShaderResources(i, 1, &mShaderResourceViews[i]);

	ID3D11ShaderResourceView* shadowMap = mPointLight->getShadowMapSrv();
	mDeviceContext->PSSetShaderResources(BUFFER_AMOUNT, 1, &shadowMap);

	// Set constant buffers.
	mDeviceContext->PSSetConstantBuffers(0, 1, &mLightBuffer);
	mDeviceContext->PSSetConstantBuffers(1, 1, &mCameraBuffer);
	mDeviceContext->PSSetConstantBuffers(2, 1, &mFrustumBuffer);

	// Draw quad to screen.
	UINT32 stride = sizeof(VertexLp);
	UINT32 offset = 0;
	mDeviceContext->IASetVertexBuffers(0, 1, &mVertexBufferLp, &stride, &offset);
	mDeviceContext->Draw(6, 0);

	mDeviceContext->PSSetSamplers(0, 0, nullptr);
	mDeviceContext->PSSetConstantBuffers(0, 0, nullptr);
	mDeviceContext->PSSetConstantBuffers(1, 0, nullptr);
	mDeviceContext->PSSetConstantBuffers(2, 0, nullptr);
	for (int i = 0; i < BUFFER_AMOUNT; i++)
		mDeviceContext->PSSetShaderResources(i, 0, nullptr);

	return hr;
}

void Renderer3D::setShaders(ID3D11InputLayout* inputLayout, ID3D11VertexShader* vs, ID3D11HullShader* hs, ID3D11DomainShader* ds, ID3D11GeometryShader* gs, ID3D11PixelShader* ps) {
	mDeviceContext->VSSetShader(vs, nullptr, 0);
	mDeviceContext->HSSetShader(hs, nullptr, 0);
	mDeviceContext->DSSetShader(ds, nullptr, 0);
	mDeviceContext->GSSetShader(gs, nullptr, 0);
	mDeviceContext->PSSetShader(ps, nullptr, 0);
	mDeviceContext->IASetInputLayout(inputLayout);
}