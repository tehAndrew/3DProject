#include "World3D.h"

World3D::World3D() {};

void World3D::onMouseMove(WPARAM button, int x, int y, int lastX, int lastY) {
	mRenderer.getCamera()->onMouseMove(button, x, y, lastX, lastY);
}

HRESULT World3D::init(HWND hWnd, int screenWidth, int screenHeight, float nearPlane, float farPlane, float cameraHeight) {
	HRESULT hr;

	mScreenWidth  = screenWidth;
	mScreenHeight = screenHeight;
	mNearPlane    = nearPlane;
	mFarPlane     = farPlane;

	// Init D3D
	if (FAILED(hr = mRenderer.initDirect3D(hWnd, mScreenWidth, mScreenHeight, mNearPlane, mFarPlane)))
		return hr;

	// Init Camera
	mRenderer.getCamera()->setView(XMVectorSet(0.f, -cameraHeight, 0.f, 1.f), XMVectorSet(0.f, -cameraHeight, -1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	mRenderer.getCamera()->setProj(XM_PI * 0.45f, (float)mScreenWidth / mScreenHeight, mNearPlane, mFarPlane);

	// Init Point light
	mRenderer.getPointLight()->setPos(XMVectorSet(0.f, 200.f, 0.f, 1.f));
	mRenderer.getPointLight()->setAmbientColor(XMVectorSet(0.05f, 0.05f, 0.05f, 1.f));
	mRenderer.getPointLight()->setDiffuseColor(XMVectorSet(0.6f, 0.6f, 1.f, 1.f));
	mRenderer.getPointLight()->setAttenuation(0.000006);

	// Place models.

	/*for (int i = 0; i < 3; i++)
		for(int j = 0; j < 3; j++)
			if (!(i == 6 && j == 6))
				mRenderer.createModelFromFile(
					L".\\Models\\statue2.aobj",
					XMVectorSet(-1200 + 200 * i, -10.f, 1200 - 200 * j, 0),
					XMVectorSet(-15 + rand() % 30, 0.f, -15 + rand() % 30, 0.f),
					XMVectorSet(1.0f, 1.0f, 1.0f, 1.f));*/

	mRenderer.createModelFromFile(
		L".\\Models\\statue2.aobj",
		XMVectorSet(-200.f, -10.f, -200.f, 0),
		XMVectorSet(0.f, 45.f, 0.f, 0.f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 1.f));

	mRenderer.createModelFromFile(
		L".\\Models\\statue2.aobj",
		XMVectorSet(0.f, -10.f, -300.f, 0),
		XMVectorSet(0.f, 0.f, 0.f, 0.f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 1.f));

	mRenderer.createModelFromFile(
		L".\\Models\\statue2.aobj",
		XMVectorSet(200.f, -10.f, -200.f, 0),
		XMVectorSet(0.f, -45.f, 0.f, 0.f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 1.f));

	mRenderer.createModelFromFile(
		L".\\Models\\statue2.aobj",
		XMVectorSet(-200.f, -10.f, 200.f, 0),
		XMVectorSet(0.f, 135.f, 0.f, 0.f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 1.f));

	mRenderer.createModelFromFile(
		L".\\Models\\statue2.aobj",
		XMVectorSet(0.f, -10.f, 300.f, 0),
		XMVectorSet(0.f, 180.f, 0.f, 0.f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 1.f));

	mRenderer.createModelFromFile(
		L".\\Models\\statue2.aobj",
		XMVectorSet(200.f, -10.f, 200.f, 0),
		XMVectorSet(0.f, -135.f, 0.f, 0.f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 1.f));

	mRenderer.createModelFromFile(
		L".\\Models\\statue2.aobj",
		XMVectorSet(300.f, -10.f, 0.f, 0),
		XMVectorSet(0.f, -90.f, 0.f, 0.f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 1.f));

	mRenderer.createModelFromFile(
		L".\\Models\\statue2.aobj",
		XMVectorSet(-300.f, -10.f, 0.f, 0),
		XMVectorSet(0.f, 90.f, 0.f, 0.f),
		XMVectorSet(1.0f, 1.0f, 1.0f, 1.f));


	//// Load Models and Generate Geometry
	//mRenderer.createModelFromFile(
	//	L".\\Models\\statue1.aobj",
	//	XMVectorSet(100, -10.f, -200, 0),
	//	XMVectorSet(0.f, 0, 0.f, 0.f),
	//	XMVectorSet(100.f, 100.f, 100.f, 0.f));

	//mRenderer.createModelFromFile(
	//	L".\\Models\\statue1.aobj",
	//	XMVectorSet(300, -10.f, -200, 0),
	//	XMVectorSet(0.f, 0, 0.f, 0.f),
	//	XMVectorSet(100.f, 100.f, 100.f, 0.f));

	//mRenderer.createModelFromFile(
	//	L".\\Models\\statue1.aobj",
	//	XMVectorSet(500, -10.f, -200, 0),
	//	XMVectorSet(0.f, 0, 0.f, 0.f),
	//	XMVectorSet(100.f, 100.f, 100.f, 0.f));

	mRenderer.createFloor(XMVectorSet(0.f, 1400.f, -1400.f, 1.f), XMVectorSet(90.f, 0.f, 0.f, 0.f), 1400, 16, XMVectorSet(0.3f, 0.3f, 0.3f, 1.f), 100, mRenderer.getTexture(L"debug.dds"));
	mRenderer.createFloor(XMVectorSet(0.f, 1400.f, 1400.f, 1.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), 1400, 16, XMVectorSet(0.3f, 0.3f, 0.3f, 1.f), 100, mRenderer.getTexture(L"debug.dds"));
	mRenderer.createFloor(XMVectorSet(-1400.f, 1400.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, -90.f, 0.f), 1400, 16, XMVectorSet(0.3f, 0.3f, 0.3f, 1.f), 100, mRenderer.getTexture(L"debug.dds"));
	mRenderer.createFloor(XMVectorSet(1400.f, 1400.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 90.f, 0.f), 1400, 16, XMVectorSet(0.3f, 0.3f, 0.3f, 1.f), 100, mRenderer.getTexture(L"debug.dds"));
	mRenderer.createFloor(XMVectorSet(0.f, 2800.f, 0.f, 1.f), XMVectorSet(180.f, 0.f, 0.f, 0.f), 1400, 16, XMVectorSet(0.3f, 0.3f, 0.3f, 1.f), 100, mRenderer.getTexture(L"debug.dds"));
	mRenderer.createFloor(XMVectorSet(0.f, 0.f, 0.f, 1.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), 1400, 16, XMVectorSet(0.3f, 0.3f, 0.3f, 1.f), 100, mRenderer.getTexture(L"debug.dds"));

	/*mRenderer.createFloor(XMVectorSet(0.f, 1000.f, -1000.f, 0.f), XMVectorSet(90.f, 0.f, 0.f, 0.f), 1000, mRenderer.getTexture("floortex"));
	mRenderer.createFloor(XMVectorSet(0.f, 1000.f, 1000.f, 0.f), XMVectorSet(-90.f, 0.f, 0.f, 0.f), 1000, mRenderer.getTexture("floortex"));
	mRenderer.createFloor(XMVectorSet(-1000.f, 1000.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, -90.f, 0.f), 1000, mRenderer.getTexture("floortex"));
	mRenderer.createFloor(XMVectorSet(1000.f, 1000.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 90.f, 0.f), 1000, mRenderer.getTexture("floortex"));
	mRenderer.createFloor(XMVectorSet(0.f, 2000.f, 0.f, 0.f), XMVectorSet(180.f, 0.f, 0.f, 0.f), 1000, mRenderer.getTexture("floortex"));
	mRenderer.createFloor(XMVectorSet(0.f, 0.f, 0.f, 0.f), XMVectorSet(0.f, 0.f, 0.f, 0.f), 1000, mRenderer.getTexture("floortex"));*/

	return hr;
}

void World3D::update(double deltaTime) {
	mRenderer.getCamera()->update(deltaTime);
	mRenderer.getPointLight()->update(deltaTime);
}

HRESULT World3D::render() {
	HRESULT hr;
	if (FAILED(hr = mRenderer.render()))
		return hr;

	return hr;
}