#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <unordered_map>
#include <d3d11.h>

#include "Texture.h"

class TextureManager {
	private:
		ID3D11SamplerState* mSamplerState;
		std::unordered_map<std::wstring, Texture*>* mTextures;

	public:
		TextureManager();
		virtual ~TextureManager();

		HRESULT initTextureManager(ID3D11Device* device);
		bool textureAlreadyLoaded(wchar_t* filename);
		HRESULT addTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, wchar_t* filename);
		Texture* getTexture(wchar_t* filename) const;
		ID3D11SamplerState* getSamplerState() const;
};

#endif