#include "ModelCreator.h"

Model* ModelCreator::loadModel(wchar_t* filename, FXMVECTOR pos, FXMVECTOR rot, FXMVECTOR scale, ID3D11Device* device, ID3D11DeviceContext* deviceContext, TextureManager* textureManager) {
	struct AOBJHeader {
		unsigned int vertexAmount;
		unsigned int indexAmount;
		wchar_t colorMapFilename[64];
		float refcR, refcG, refcB;
		float gloss;
	};
	AOBJHeader header;
	
	HRESULT hr;
	Model* model = new Model();
	std::vector<Vertex>* vertices = new std::vector<Vertex>();
	std::vector<unsigned int>* indices  = new std::vector<unsigned int>();

	std::ifstream file;

	file.open(filename, std::ios::in | std::ios::binary);
	if (file.is_open()) {
		file.read((char*) &header, sizeof(AOBJHeader));
		for (unsigned int i = 0; i < header.vertexAmount; i++) {
			Vertex tempVertex;
			file.read((char*)&tempVertex, sizeof(Vertex));
			tempVertex.v = 1 - tempVertex.v;
			vertices->push_back(tempVertex);
		}
		for (unsigned int i = 0; i < header.indexAmount; i++) {
			unsigned int tempIndex;
			file.read((char*)&tempIndex, sizeof(unsigned int));
			indices->push_back(tempIndex);
		}
	}

	Texture* texture;
	if (!textureManager->textureAlreadyLoaded(header.colorMapFilename))
		textureManager->addTexture(device, deviceContext, header.colorMapFilename);
	texture = textureManager->getTexture(header.colorMapFilename);

	MaterialData material;
	material.reflectiveColor = XMVectorSet(header.refcR, header.refcG, header.refcB, 1.f);
	material.gloss = header.gloss;

	if (FAILED(hr = model->defineGeometry(vertices, indices, device, texture, material))) {
		delete vertices;
		delete indices;
		delete model;
		return nullptr;
	}

	model->setPos(pos);
	model->setRot(rot);
	model->setScale(scale);

	delete vertices;
	delete indices;
	return model;
}

Model* ModelCreator::generateFloor(FXMVECTOR pos, FXMVECTOR rotation, float side, ID3D11Device* device, float detail, FXMVECTOR reflectiveColor, float gloss, Texture* texture) {
	HRESULT hr;
	Model* model = new Model();
	MaterialData material;
	std::vector<Vertex>* vertices = new std::vector<Vertex>();
	std::vector<UINT32>* indices  = new std::vector<UINT32>();
	
	material.reflectiveColor = reflectiveColor;
	material.gloss           = gloss;
	
	float lenPerSub = 2.f / detail;

	// for n edges there is for n + 1 vertices for each row and column.
	for (int x = 0; x < detail; x++) {
		for (int z = 0; z < detail; z++) {
			vertices->push_back({ -1.f + lenPerSub * x, 0.f, -1.f + lenPerSub * z, 0.f, 1.f, 0.f, 0.f, 1.f});
			vertices->push_back({ -1.f + lenPerSub * x, 0.f, -1.f + lenPerSub * (z + 1), 0.f, 1.f, 0.f, 0.f, 0.f });
			vertices->push_back({ -1.f + lenPerSub * (x + 1), 0.f, -1.f + lenPerSub * (z + 1), 0.f, 1.f, 0.f, 1.f, 0.f });
			vertices->push_back({ -1.f + lenPerSub * (x + 1), 0.f, -1.f + lenPerSub * z, 0.f, 1.f, 0.f, 1.f, 1.f });
		}
	}

	for (int i = 0; i < (detail * 4)*(detail * 4); i += 4) {
		indices->push_back(i);       // Lower left
		indices->push_back(i + 1);   // Upper left
		indices->push_back(i + 2);   // Upper right
		indices->push_back(i);       // Lower left
		indices->push_back(i + 2);   // Upper right
		indices->push_back(i + 3);   // lower right
	}

	if (FAILED(hr = model->defineGeometry(vertices, indices, device, texture, material))) {
		delete vertices;
		delete indices;
		delete model;
		return nullptr;
	}

	model->setPos(pos);
	model->setRot(rotation);
	model->setScale(XMVectorSet(side, side, side, 0.f));

	delete vertices;
	delete indices;
	return model;
}

//Model* ModelCreator::generateCube(FXMVECTOR pos, FXMVECTOR rotation, float side, ID3D11Device* device, Texture* texture) {
//	HRESULT hr;
//	Model* model = new Model();
//	std::vector<Vertex>* vertices = new std::vector<Vertex>();
//	std::vector<UINT32>* indices = new std::vector<UINT32>();
//	
//	// Mesh data
//	vertices->push_back({-0.5f, -0.5f, -0.5f, 0.f, 1.f});
//	vertices->push_back({-0.5f, +0.5f, -0.5f, 0.f, 0.f});
//	vertices->push_back({+0.5f, +0.5f, -0.5f, 1.f, 0.f});
//	vertices->push_back({+0.5f, -0.5f, -0.5f, 1.f, 1.f});
//	vertices->push_back({-0.5f, -0.5f, +0.5f, 1.f, 1.f});
//	vertices->push_back({-0.5f, +0.5f, +0.5f, 1.f, 0.f});
//	vertices->push_back({+0.5f, +0.5f, +0.5f, 0.f, 0.f});
//	vertices->push_back({+0.5f, -0.5f, +0.5f, 0.f, 1.f});
//
//	//face 1
//	indices->push_back(0);
//	indices->push_back(1);
//	indices->push_back(2);
//	indices->push_back(0);
//	indices->push_back(2);
//	indices->push_back(3);
//	//face 2
//	indices->push_back(4);
//	indices->push_back(6);
//	indices->push_back(5);
//	indices->push_back(4);
//	indices->push_back(7);
//	indices->push_back(6);
//	//face 3
//	indices->push_back(4);
//	indices->push_back(5);
//	indices->push_back(1);
//	indices->push_back(4);
//	indices->push_back(1);
//	indices->push_back(0);
//	//face 4
//	indices->push_back(3);
//	indices->push_back(2);
//	indices->push_back(6);
//	indices->push_back(3);
//	indices->push_back(6);
//	indices->push_back(7);
//	//face 5
//	indices->push_back(1);
//	indices->push_back(5);
//	indices->push_back(6);
//	indices->push_back(1);
//	indices->push_back(6);
//	indices->push_back(2);
//	//face 6
//	indices->push_back(4);
//	indices->push_back(0);
//	indices->push_back(3);
//	indices->push_back(4);
//	indices->push_back(3);
//	indices->push_back(7);
//
//	if (FAILED(hr = model->defineGeometry(vertices, indices, device, texture))) {
//		delete vertices;
//		delete indices;
//		delete model;
//		return nullptr;
//	}
//
//	model->setPos(pos);
//	model->setRot(rotation);
//	model->setScale(XMVectorSet(side, side, side, 0.f));
//
//	delete vertices;
//	delete indices;
//	return model;
//}