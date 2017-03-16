#ifndef SHADERHANDLER_H
#define SHADERHANDLER_H

#include <forward_list>

#include "Renderer3D.h"

class ShaderHandler {
	private
		ID3D11ShaderResourceView* 
	public:
		ShaderHandler();
		virtual ~ShaderHandler();

		HRESULT initShaders();


};

#endif