#include "TextureManager.h"

TextureManager::TextureManager() {
	ID3D11SamplerState* mSamplerState = nullptr;
	mTextures = new std::unordered_map<std::wstring, Texture*>();
}

TextureManager::~TextureManager() {
	if (mSamplerState != nullptr)
		mSamplerState->Release();

	for (auto itr = mTextures->begin(); itr != mTextures->end(); ++itr)
		delete itr->second;

	delete mTextures;
}

HRESULT TextureManager::initTextureManager(ID3D11Device* device) {
	HRESULT hr;
	D3D11_SAMPLER_DESC sDesc;
	ZeroMemory(&sDesc, sizeof(sDesc));

	sDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;

	if (FAILED(hr = device->CreateSamplerState(&sDesc, &mSamplerState)))
		return hr;

	return hr;
}

bool TextureManager::textureAlreadyLoaded(wchar_t* filename) {
	std::wstring key(filename);
	return mTextures->count(key);
}

HRESULT TextureManager::addTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, wchar_t* filename) {
	HRESULT hr;
	Texture* tempTex = new Texture();
	if (FAILED(hr = tempTex->loadTexture(device, deviceContext, filename)))
		return hr;

	std::wstring key(filename);
	std::pair<std::wstring, Texture*> texPair(key, tempTex);
	mTextures->insert(texPair);

	return hr;
}

Texture* TextureManager::getTexture(wchar_t* filename) const {
	std::wstring key(filename);
	return mTextures->at(key);
}

ID3D11SamplerState* TextureManager::getSamplerState() const {
	return mSamplerState;
}