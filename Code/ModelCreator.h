#ifndef MODELCREATOR_H
#define MODELCREATOR_H

#include <DirectXMath.h>
#include <vector>
#include <forward_list>
#include <string>
#include <fstream>
#include <sstream>

#include "Model.h"

using namespace DirectX;

class ModelCreator {
	public:
		static Model* loadModel(wchar_t* filename, FXMVECTOR pos, FXMVECTOR rot, FXMVECTOR scale, ID3D11Device* device, Texture* texture);
		static Model* generateFloor(FXMVECTOR pos, FXMVECTOR rotation, float side, ID3D11Device* device, Texture* texture);
		static Model* generateCube(FXMVECTOR pos, FXMVECTOR rotation, float side, ID3D11Device* device, Texture* texture);
};

#endif