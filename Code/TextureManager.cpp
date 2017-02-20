#include "TextureManager.h"

TextureManager::TextureManager() {
	ID3D11SamplerState* mSamplerState = nullptr;
	mTextures = new std::unordered_map<std::string, Texture*>();
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

HRESULT TextureManager::addTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, wchar_t* filename, std::string name) {
	HRESULT hr;
	Texture* tempTex = new Texture();
	if (FAILED(hr = tempTex->loadTexture(device, deviceContext, filename)))
		return hr;

	std::pair<std::string, Texture*> texPair(name, tempTex);
	mTextures->insert(texPair);

	return hr;
}

Texture* TextureManager::getTexture(std::string name) const {
	return mTextures->at(name);
}

ID3D11SamplerState* TextureManager::getSamplerState() const {
	return mSamplerState;
}