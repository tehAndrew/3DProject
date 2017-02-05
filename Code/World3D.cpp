#include "World3D.h"

World3D::World3D() {};

void World3D::onMouseMove(WPARAM button, int x, int y, int lastX, int lastY) {
	mCamera.onMouseMove(button, x, y, lastX, lastY);
}

HRESULT World3D::init(HWND hWnd, int screenWidth, int screenHeight, float cameraHeight, bool flying) {
	HRESULT hr;

	mScreenWidth  = screenWidth;
	mScreenHeight = screenHeight;
	
	mCamera.setFlyingMode(flying);
	mCamera.setView(XMVectorSet(0.f, -cameraHeight, 0.f, 1.f), XMVectorSet(0.f, -cameraHeight, -1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	mCamera.setProj(XM_PI * 0.45f, (float)mScreenWidth / mScreenHeight, 0.1f, 800.f);

	if (FAILED(hr = mRenderer.initDirect3D(hWnd, mScreenWidth, mScreenHeight)))
		return hr;

	return hr;
}

void World3D::update(double deltaTime) {
	mCamera.update(deltaTime);
}

HRESULT World3D::render() {
	HRESULT hr;
	mRenderer.setMatrices(XMMatrixIdentity(), mCamera.getViewMatrix(), mCamera.getProjMatrix());
	if (FAILED(hr = mRenderer.render()))
		return hr;

	return hr;
}