#include "Model.h"

#include <cstdlib>

Model::Model() {
	mVertexBuffer = nullptr;
	mIndexBuffer = nullptr;

	mVertexAmount = 0;
	mIndexAmount = 0;

	mX = 0.f;
	mY = 0.f;
	mZ = 0.f;

	mScaleX = 1.f;
	mScaleY = 1.f;
	mScaleZ = 1.f;

	mRotX = 0.f;
	mRotY = 0.f;
	mRotZ = 0.f;

	mTexture = nullptr;
}

Model::~Model() {
	if (mVertexBuffer != nullptr)
		mVertexBuffer->Release();
	if (mIndexBuffer != nullptr)
		mIndexBuffer->Release();
}

void Model::setPos(FXMVECTOR pos) {
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, pos);

	mX = temp.x;
	mY = temp.y;
	mZ = temp.z;
}

void Model::setScale(FXMVECTOR scale) {
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, scale);

	mScaleX = temp.x;
	mScaleY = temp.y;
	mScaleZ = temp.z;
}

void Model::setRot(FXMVECTOR rot) {
	XMFLOAT3 temp;
	XMStoreFloat3(&temp, rot);

	mRotX = temp.x;
	mRotY = temp.y;
	mRotZ = temp.z;
}

XMMATRIX Model::getWorldMatrix() const {
	XMMATRIX worldMatrix = XMMatrixScaling(mScaleX, mScaleY, mScaleZ) *
						   XMMatrixRotationX(XMConvertToRadians(mRotX)) * 
						   XMMatrixRotationY(XMConvertToRadians(mRotY)) * 
						   XMMatrixRotationZ(XMConvertToRadians(mRotZ)) * 
						   XMMatrixTranslation(mX, mY, mZ);
	
	return XMMatrixTranspose(worldMatrix);
}

HRESULT Model::defineGeometry(std::vector<Vertex> *vertices, std::vector<UINT32> *indices, ID3D11Device* device, Texture* texture) {
	mVertexAmount = vertices->size();
	mIndexAmount  = indices->size();
	
	// VERTEX BUFFER
	// Buffer descriptions
	HRESULT hr;

	D3D11_BUFFER_DESC vbDesc;
	ZeroMemory(&vbDesc, sizeof(D3D11_BUFFER_DESC));

	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.Usage     = D3D11_USAGE_DEFAULT;
	vbDesc.ByteWidth = sizeof(Vertex) * vertices->size();

	D3D11_SUBRESOURCE_DATA vdata;
	vdata.pSysMem = const_cast<Vertex*>(vertices->data());

	// Create the vertex buffer.
	if (FAILED(hr = device->CreateBuffer(&vbDesc, &vdata, &mVertexBuffer))) {
		return hr;
	}

	// INDEX BUFFER
	// Buffer descriptions
	D3D11_BUFFER_DESC ibDesc;
	ZeroMemory(&ibDesc, sizeof(D3D11_BUFFER_DESC));

	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(UINT32) * indices->size();

	D3D11_SUBRESOURCE_DATA idata;
	idata.pSysMem = const_cast<UINT32*>(indices->data());

	// Create the vertex buffer.
	if (FAILED(hr = device->CreateBuffer(&ibDesc, &idata, &mIndexBuffer))) {
		return hr;
	}
	
	mTexture = texture;

	return hr;
}

void Model::render(ID3D11DeviceContext* deviceContext) {
	// Set vertex buffer stride and offset.
	UINT32 stride = sizeof(Vertex);
	UINT32 offset = 0;

	ID3D11ShaderResourceView *srvTemp = mTexture->getTexture();
	deviceContext->PSSetShaderResources(0, 1, &srvTemp);

	// Set the vertex buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);

	// Set the index buffer to active in the input assembler so it can be rendered.
	deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->DrawIndexed(mIndexAmount, 0, 0);
}