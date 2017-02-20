/*Code based on code from rastertek.com*/

#ifndef TEXTURE_H
#define TEXTURE_H

#include <stdio.h>
#include <assert.h>

#include <d3d11.h>

#include <DDSTextureLoader.h>

using namespace DirectX;

class Texture {
	private:
		ID3D11Texture2D*		  mTexture;
		ID3D11ShaderResourceView* mTextureView;
		bool mTextureLoaded;

	public:
		Texture();
		virtual ~Texture();

		HRESULT loadTexture(ID3D11Device* device, ID3D11DeviceContext* deviceContext, wchar_t* filename);
		ID3D11ShaderResourceView* getTexture() const;
};

#endif