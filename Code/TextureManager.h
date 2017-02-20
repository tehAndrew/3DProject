#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <unordered_map>
#include <d3d11.h>

#include "Texture.h"

class TextureManager {
	private:
		ID3D11SamplerState* mSamplerState;
		std::unordered_map<std::string, Texture*>* mTextures;

	public:
		TextureManager();
		virtual ~TextureManager();

		HRESULT initTextureManager(ID3D11Device* device);
		HRESULT addTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, wchar_t* filename, std::string name);
		Texture* getTexture(std::string name) const;
		ID3D11SamplerState* getSamplerState() const;
};

#endif