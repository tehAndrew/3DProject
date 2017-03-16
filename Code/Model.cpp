#include "Model.h"

#include <cstdlib>

Model::Model() {
	mVertexBuffer   = nullptr;
	mIndexBuffer    = nullptr;
	mMaterialBuffer = nullptr;

	mVertexAmount = 0;
	mIndexAmount  = 0;

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

	if (mMaterialBuffer != nullptr)
		mMaterialBuffer->Release();
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

ID3D11ShaderResourceView* Model::getTexture() const {
	return mTexture->getTexture();
}

ID3D11Buffer* Model::getMaterial() const {
	return mMaterialBuffer;
}

HRESULT Model::defineGeometry(std::vector<Vertex> *vertices, std::vector<UINT32> *indices, ID3D11Device* device, Texture* texture, MaterialData material) {
	mVertexAmount = vertices->size();
	mIndexAmount  = indices->size();
	mMaterial = material;

	HRESULT hr;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc, materialBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData, materialData;
	ZeroMemory(&vertexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&indexBufferDesc, sizeof(D3D11_BUFFER_DESC));
	ZeroMemory(&materialBufferDesc, sizeof(D3D11_BUFFER_DESC));
	
	// Create vertex buffer.
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.Usage     = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * vertices->size();

	vertexData.pSysMem = const_cast<Vertex*>(vertices->data());

	if (FAILED(hr = device->CreateBuffer(&vertexBufferDesc, &vertexData, &mVertexBuffer)))
		return hr;

	// Create index buffer.
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.Usage     = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned int) * indices->size();

	indexData.pSysMem = const_cast<unsigned int*>(indices->data());

	if (FAILED(hr = device->CreateBuffer(&indexBufferDesc, &indexData, &mIndexBuffer)))
		return hr;

	// Create material buffer.
	materialBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	materialBufferDesc.Usage     = D3D11_USAGE_DEFAULT;
	materialBufferDesc.ByteWidth = sizeof(MaterialData);

	materialData.pSysMem = const_cast<MaterialData*>(&mMaterial);

	if (FAILED(hr = device->CreateBuffer(&materialBufferDesc, &materialData, &mMaterialBuffer)))
		return hr;
	
	mTexture = texture;

	return hr;
}

void Model::render(ID3D11DeviceContext* deviceContext) {
	// Set vertex buffer stride and offset.
	UINT32 stride = sizeof(Vertex);
	UINT32 offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &mVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(mIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	deviceContext->DrawIndexed(mIndexAmount, 0, 0);
}