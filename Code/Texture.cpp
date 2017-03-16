#include "Texture.h"

Texture::Texture() {
	mTexture = nullptr;
	mTextureView = nullptr;
}

Texture::~Texture() {
	if (mTexture != nullptr)
		mTexture->Release();

	if (mTextureView != nullptr)
		mTextureView->Release();
}

HRESULT Texture::loadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, wchar_t* filename) {
	// Load texture can only be called once for a Texture object.
	assert(!mTextureLoaded);
	HRESULT hr;
	wchar_t fullFilename[128] = L".\\Models\\";
	wcscat(fullFilename, filename);

	if (FAILED(hr = CreateDDSTextureFromFile(device, deviceContext, fullFilename, (ID3D11Resource**) &mTexture, &mTextureView)))
		return hr;

	mTextureLoaded = true;
	return hr;
}

ID3D11ShaderResourceView* Texture::getTexture() const {
	return mTextureView;
}