#include "World3D.h"

World3D::World3D() {};

void World3D::onMouseMove(WPARAM button, int x, int y, int lastX, int lastY) {
	mRenderer.getCamera()->onMouseMove(button, x, y, lastX, lastY);
}

HRESULT World3D::init(HWND hWnd, int screenWidth, int screenHeight, float cameraHeight) {
	HRESULT hr;

	mScreenWidth  = screenWidth;
	mScreenHeight = screenHeight;

	// Init D3D
	if (FAILED(hr = mRenderer.initDirect3D(hWnd, mScreenWidth, mScreenHeight)))
		return hr;

	// Init Camera
	mRenderer.getCamera()->setView(XMVectorSet(0.f, -cameraHeight, 0.f, 1.f), XMVectorSet(0.f, -cameraHeight, -1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	mRenderer.getCamera()->setProj(XM_PI * 0.45f, (float)mScreenWidth / mScreenHeight, 0.1f, 20000.f);

	// Load Textures
	mRenderer.createTexture(L".\\Textures\\statue1.dds", "statue1tex");
	mRenderer.createTexture(L".\\Textures\\statue2.dds", "statue2tex");
	mRenderer.createTexture(L".\\Textures\\doomTexture1.dds", "floortex");
	mRenderer.createTexture(L".\\Textures\\leaves.dds", "debug");
	
	// Load Models and Generate Geometry
	mRenderer.createModelFromFile(
		L".\\Models\\statue1.aobj",
		XMVectorSet(100, -10.f, -200, 0),
		XMVectorSet(0.f, 0, 0.f, 0.f),
		XMVectorSet(100.f, 100.f, 100.f, 0.f),
		mRenderer.getTexture("statue1tex"));

	mRenderer.createModelFromFile(
		L".\\Models\\statue2.aobj",
		XMVectorSet(-100, 0.f, -200, 0),
		XMVectorSet(0.f, 0, 0.f, 0.f),
		XMVectorSet(2.f, 2.f, 2.f, 0.f),
		mRenderer.getTexture("statue2tex"));

	mRenderer.createFloor(XMVectorSet(0.f, 1000.f, -1000.f, 0.f), XMVectorSet(90.f, 0.f, 0.f, 0.f), 1000, mRenderer.getTexture("floortex"));
	mRenderer.createFloor(XMVectorSet(0.f, 1000.f, 1000.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), 1000, mRenderer.getTexture("floortex"));
	mRenderer.createFloor(XMVectorSet(-1000.f, 1000.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, -90.f, 0.f), 1000, mRenderer.getTexture("floortex"));
	mRenderer.createFloor(XMVectorSet(1000.f, 1000.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 90.f, 0.f), 1000, mRenderer.getTexture("floortex"));
	mRenderer.createFloor(XMVectorSet(0.f, 2000.f, 0.f, 0.f), XMVectorSet(180.f, 0.f, 0.f, 0.f), 1000, mRenderer.getTexture("floortex"));
	mRenderer.createFloor(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), 1000, mRenderer.getTexture("floortex"));

	return hr;
}

void World3D::update(double deltaTime) {
	mRenderer.getCamera()->update(deltaTime);
}

HRESULT World3D::render() {
	HRESULT hr;
	if (FAILED(hr = mRenderer.render()))
		return hr;

	return hr;
}