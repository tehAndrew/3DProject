#ifndef WORLD3D_H
#define WORLD3D_H

#include <DirectXMath.h>

#include "Renderer3D.h"
#include "FirstPersonCamera.h"

using namespace DirectX;

class World3D {
	private:
		FirstPersonCamera mCamera;
		Renderer3D		  mRenderer;

		int mScreenWidth;
		int mScreenHeight;

	public:
		World3D();
		virtual ~World3D() {};

		// Event
		void onMouseMove(WPARAM button, int x, int y, int lastX, int lastY);

		HRESULT init(HWND hWnd, int screenWidth, int screenHeight, float cameraHeight, bool flying);
		void update(double deltaTime);
		HRESULT render();


};

#endif