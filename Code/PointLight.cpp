#include "PointLight.h"

PointLight::PointLight() {
	mTick = 0;

	mPos          = {0.f, 0.f, 0.f, 1.f};
	mAmbientColor = {0.1f, 0.1f, 0.1f, 1.f};
	mDiffuseColor = {1.f, 1.f, 1.f, 1.f};
	mAttenuation  = 0.000001f;

	mMatrixBuffer = nullptr;
	mShadowMapSrv = nullptr;

	for (int i = 0; i < 6; i++)
		mShadowMapDsvs[i] = nullptr;
}

PointLight::~PointLight() {
	if (mMatrixBuffer != nullptr)
		mMatrixBuffer->Release();

	if (mShadowMapSrv != nullptr)
		mShadowMapSrv->Release();

	for (int i = 0; i < 6; i++)
		if (mShadowMapDsvs[i] != nullptr)
			mShadowMapDsvs[i]->Release();
}

HRESULT PointLight::initPointLight(ID3D11Device* device, float nearPlane, float farPlane) {
	HRESULT hr;
	D3D11_TEXTURE2D_DESC shadowMapDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC shadowMapSrvDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC shadowMapDsvDesc;
	D3D11_BUFFER_DESC cbMatrices;
	ID3D11Texture2D* shadowMap;

	// Set up matrices.
	XMStoreFloat4x4(&mViewMatrices[0], XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&mPos), XMLoadFloat4(&mPos) + XMVectorSet(1.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
	XMStoreFloat4x4(&mViewMatrices[1], XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&mPos), XMLoadFloat4(&mPos) + XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
	XMStoreFloat4x4(&mViewMatrices[2], XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&mPos), XMLoadFloat4(&mPos) + XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
	XMStoreFloat4x4(&mViewMatrices[3], XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&mPos), XMLoadFloat4(&mPos) + XMVectorSet(0.f, -1.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
	XMStoreFloat4x4(&mViewMatrices[4], XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&mPos), XMLoadFloat4(&mPos) + XMVectorSet(0.f, 0.f, 1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
	XMStoreFloat4x4(&mViewMatrices[5], XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&mPos), XMLoadFloat4(&mPos) + XMVectorSet(0.f, 0.f, -1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
	XMStoreFloat4x4(&mProjMatrix, XMMatrixTranspose(XMMatrixPerspectiveFovLH(XM_PI / 2.f, (float)mShadowMapRes / (float)mShadowMapRes, nearPlane, farPlane)));

	// Set up view port.
	mViewPort.TopLeftX = 0.f;
	mViewPort.TopLeftY = 0.f;
	mViewPort.Width    = (float) mShadowMapRes;
	mViewPort.Height   = (float) mShadowMapRes;
	mViewPort.MinDepth = 0.f;
	mViewPort.MaxDepth = 1.f;

	// Describe the texture.
	shadowMapDesc.Width				 = mShadowMapRes;
	shadowMapDesc.Height			 = mShadowMapRes;
	shadowMapDesc.MipLevels			 = 1;
	shadowMapDesc.ArraySize			 = 6;
	shadowMapDesc.Format			 = DXGI_FORMAT_R24G8_TYPELESS;
	shadowMapDesc.SampleDesc.Count   = 1;
	shadowMapDesc.SampleDesc.Quality = 0;
	shadowMapDesc.Usage				 = D3D11_USAGE_DEFAULT;
	shadowMapDesc.BindFlags			 = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	shadowMapDesc.CPUAccessFlags	 = 0;
	shadowMapDesc.MiscFlags			 = D3D11_RESOURCE_MISC_TEXTURECUBE;

	// Create shadow map texture.
	if (FAILED(hr = device->CreateTexture2D(&shadowMapDesc, 0, &shadowMap)))
		return hr;

	// Describe the srv.
	shadowMapSrvDesc.Format                      = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	shadowMapSrvDesc.ViewDimension               = D3D11_SRV_DIMENSION_TEXTURECUBE;
	shadowMapSrvDesc.TextureCube.MipLevels       = 1;
	shadowMapSrvDesc.TextureCube.MostDetailedMip = 0;

	// Create srv.
	if (FAILED(hr = device->CreateShaderResourceView(shadowMap, &shadowMapSrvDesc, &mShadowMapSrv)))
		return hr;

	for (int i = 0; i < 6; i++) {
		// Describe the dsv.
		shadowMapDsvDesc.Flags = 0;
		shadowMapDsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		shadowMapDsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		shadowMapDsvDesc.Texture2DArray.MipSlice = 0;
		shadowMapDsvDesc.Texture2DArray.ArraySize = 1;
		shadowMapDsvDesc.Texture2DArray.FirstArraySlice = i;

		// Create dsv.
		if (FAILED(hr = device->CreateDepthStencilView(shadowMap, &shadowMapDsvDesc, &mShadowMapDsvs[i])))
			return hr;
	}

	// Describe constant buffer.
	cbMatrices.Usage = D3D11_USAGE_DYNAMIC;
	cbMatrices.ByteWidth = sizeof(ShadowMatrixData);
	cbMatrices.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	cbMatrices.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	cbMatrices.MiscFlags = 0;
	cbMatrices.StructureByteStride = 0;

	// Create constant buffer
	if (FAILED(hr = device->CreateBuffer(&cbMatrices, nullptr, &mMatrixBuffer)))
		return hr;

	return hr;
}

HRESULT PointLight::mapMatrices(ID3D11DeviceContext* deviceContext, FXMMATRIX worldMatrix, int viewIndex) {
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE data;

	mMatrices.worldMatrix = worldMatrix;
	mMatrices.viewMatrix  = XMLoadFloat4x4(&mViewMatrices[viewIndex]);
	mMatrices.projMatrix  = XMLoadFloat4x4(&mProjMatrix);

	if (FAILED(hr = deviceContext->Map(mMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &data)))
		return hr;

	memcpy(data.pData, &mMatrices, sizeof(ShadowMatrixData));
	deviceContext->Unmap(mMatrixBuffer, 0);

	return hr;
}

void PointLight::setPos(FXMVECTOR pos) {
	XMStoreFloat4(&mPos, pos);
	XMStoreFloat4x4(&mViewMatrices[0], XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&mPos), XMLoadFloat4(&mPos) + XMVectorSet(1.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
	XMStoreFloat4x4(&mViewMatrices[1], XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&mPos), XMLoadFloat4(&mPos) + XMVectorSet(-1.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
	XMStoreFloat4x4(&mViewMatrices[2], XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&mPos), XMLoadFloat4(&mPos) + XMVectorSet(0.f, 1.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 1.f, 0.f))));
	XMStoreFloat4x4(&mViewMatrices[3], XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&mPos), XMLoadFloat4(&mPos) + XMVectorSet(0.f, -1.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, -1.f, 0.f))));
	XMStoreFloat4x4(&mViewMatrices[4], XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&mPos), XMLoadFloat4(&mPos) + XMVectorSet(0.f, 0.f, 1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
	XMStoreFloat4x4(&mViewMatrices[5], XMMatrixTranspose(XMMatrixLookAtLH(XMLoadFloat4(&mPos), XMLoadFloat4(&mPos) + XMVectorSet(0.f, 0.f, -1.f, 0.f), XMVectorSet(0.f, 1.f, 0.f, 0.f))));
}

void PointLight::setAmbientColor(FXMVECTOR ambientColor) {
	XMStoreFloat4(&mAmbientColor, ambientColor);
}

void PointLight::setDiffuseColor(FXMVECTOR diffuseColor) {
	XMStoreFloat4(&mDiffuseColor, diffuseColor);
}

void PointLight::setAttenuation(float attenuation) {
	mAttenuation = attenuation;
}

FXMVECTOR PointLight::getPos() const {
	return XMLoadFloat4(&mPos);
}

FXMVECTOR PointLight::getAmbientColor() const {
	return XMLoadFloat4(&mAmbientColor);
}

FXMVECTOR PointLight::getDiffuseColor() const {
	return XMLoadFloat4(&mDiffuseColor);
}

float PointLight::getAttenuation() const {
	return mAttenuation;
}

D3D11_VIEWPORT PointLight::getViewPort() const {
	return mViewPort;
}

ID3D11Buffer* PointLight::getMatrixBuffer() const {
	return mMatrixBuffer;
}

ID3D11ShaderResourceView* PointLight::getShadowMapSrv() const {
	return mShadowMapSrv;
}

ID3D11DepthStencilView* PointLight::getShadowMapDsv(int index) const {
	return mShadowMapDsvs[index];
}

void PointLight::update(float dt) {
	setPos(XMLoadFloat4(&mPos) + XMVectorSet(0.f, sin(mTick) * 2, 0.f, 0.f));
	mTick += dt;
}