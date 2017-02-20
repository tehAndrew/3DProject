#include "ModelCreator.h"

Model* ModelCreator::loadModel(wchar_t* filename, FXMVECTOR pos, FXMVECTOR rot, FXMVECTOR scale, ID3D11Device* device, Texture* texture) {
	HRESULT hr;
	Model* model = new Model();
	std::vector<Vertex>* vertices = new std::vector<Vertex>();
	std::vector<UINT32>* indices  = new std::vector<UINT32>();

	bool readVertices = true;

	std::ifstream file(filename);
	std::string line;
	if (file.is_open()) {
		while (std::getline(file, line)) {
			if (line.substr(0, 8) == "Vertices")
				readVertices = true;
			else if (line.substr(0, 7) == "Indices")
				readVertices = false;
			else {
				if (readVertices) {
					Vertex vertex;
					std::istringstream vertexData;
					vertexData.str(line);
					vertexData >> vertex.x >> vertex.y >> vertex.z >> vertex.nx >> vertex.ny >> vertex.nz >> vertex.u >> vertex.v;
					vertex.v = 1 - vertex.v;
					vertices->push_back(vertex);
				}
				else {
					if (line.substr(0, 1) == "3") {
						UINT32 primitive, indx1, indx2, indx3;
						std::istringstream indexData;
						indexData.str(line);
						indexData >> primitive >> indx1 >> indx2 >> indx3;

						indices->push_back(indx1);
						indices->push_back(indx2);
						indices->push_back(indx3);
					}
					else if (line.substr(0, 1) == "4") {
						UINT32 primitive, indx1, indx2, indx3, indx4, indx5, indx6;
						std::istringstream indexData;
						indexData.str(line);
						indexData >> primitive >> indx1 >> indx2 >> indx3 >> indx4 >> indx5 >> indx6;

						indices->push_back(indx1);
						indices->push_back(indx2);
						indices->push_back(indx3);
						indices->push_back(indx4);
						indices->push_back(indx5);
						indices->push_back(indx6);
					}
				}
			}
		}
	}

	if (FAILED(hr = model->defineGeometry(vertices, indices, device, texture))) {
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

Model* ModelCreator::generateFloor(FXMVECTOR pos, FXMVECTOR rotation, float side, ID3D11Device* device, Texture* texture) {
	HRESULT hr;
	Model* model = new Model();
	std::vector<Vertex>* vertices = new std::vector<Vertex>();
	std::vector<UINT32>* indices  = new std::vector<UINT32>();
	
	float tess = 4.f;
	float lenPerSub = 2.f / tess;

	// for n edges there is for n + 1 vertices for each row and column.
	for (int x = 0; x < tess; x++) {
		for (int z = 0; z < tess; z++) {
			vertices->push_back({ -1.f + lenPerSub * x, 0.f, -1.f + lenPerSub * z, 0.f, 1.f, 0.f, 0.f, 1.f});
			vertices->push_back({ -1.f + lenPerSub * x, 0.f, -1.f + lenPerSub * (z + 1), 0.f, 1.f, 0.f, 0.f, 0.f });
			vertices->push_back({ -1.f + lenPerSub * (x + 1), 0.f, -1.f + lenPerSub * (z + 1), 0.f, 1.f, 0.f, 1.f, 0.f });
			vertices->push_back({ -1.f + lenPerSub * (x + 1), 0.f, -1.f + lenPerSub * z, 0.f, 1.f, 0.f, 1.f, 1.f });
		}
	}

	for (int i = 0; i < (tess * 4)*(tess * 4); i += 4) {
		indices->push_back(i);       // Lower left
		indices->push_back(i + 1); // Upper left
		indices->push_back(i + 2); // Upper right
		indices->push_back(i);       // Lower left
		indices->push_back(i + 2); // Upper right
		indices->push_back(i + 3); // lower right
	}

	if (FAILED(hr = model->defineGeometry(vertices, indices, device, texture))) {
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

Model* ModelCreator::generateCube(FXMVECTOR pos, FXMVECTOR rotation, float side, ID3D11Device* device, Texture* texture) {
	HRESULT hr;
	Model* model = new Model();
	std::vector<Vertex>* vertices = new std::vector<Vertex>();
	std::vector<UINT32>* indices = new std::vector<UINT32>();
	
	// Mesh data
	vertices->push_back({-0.5f, -0.5f, -0.5f, 0.f, 1.f});
	vertices->push_back({-0.5f, +0.5f, -0.5f, 0.f, 0.f});
	vertices->push_back({+0.5f, +0.5f, -0.5f, 1.f, 0.f});
	vertices->push_back({+0.5f, -0.5f, -0.5f, 1.f, 1.f});
	vertices->push_back({-0.5f, -0.5f, +0.5f, 1.f, 1.f});
	vertices->push_back({-0.5f, +0.5f, +0.5f, 1.f, 0.f});
	vertices->push_back({+0.5f, +0.5f, +0.5f, 0.f, 0.f});
	vertices->push_back({+0.5f, -0.5f, +0.5f, 0.f, 1.f});

	//face 1
	indices->push_back(0);
	indices->push_back(1);
	indices->push_back(2);
	indices->push_back(0);
	indices->push_back(2);
	indices->push_back(3);
	//face 2
	indices->push_back(4);
	indices->push_back(6);
	indices->push_back(5);
	indices->push_back(4);
	indices->push_back(7);
	indices->push_back(6);
	//face 3
	indices->push_back(4);
	indices->push_back(5);
	indices->push_back(1);
	indices->push_back(4);
	indices->push_back(1);
	indices->push_back(0);
	//face 4
	indices->push_back(3);
	indices->push_back(2);
	indices->push_back(6);
	indices->push_back(3);
	indices->push_back(6);
	indices->push_back(7);
	//face 5
	indices->push_back(1);
	indices->push_back(5);
	indices->push_back(6);
	indices->push_back(1);
	indices->push_back(6);
	indices->push_back(2);
	//face 6
	indices->push_back(4);
	indices->push_back(0);
	indices->push_back(3);
	indices->push_back(4);
	indices->push_back(3);
	indices->push_back(7);

	if (FAILED(hr = model->defineGeometry(vertices, indices, device, texture))) {
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