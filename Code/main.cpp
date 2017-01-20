#include <windowsx.h>
#include <DirectXMath.h> // temp

#include "Input.h"
#include "Rasterizer.h"
#include "PerformanceClock.h"
#include "FirstPersonCamera.h"

using namespace DirectX; // temp

// Forward declaration
LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND initWindow(HINSTANCE hInstance);
HRESULT initApp(HWND hWnd, Rasterizer& rasterizer, int nCmdShow);

FirstPersonCamera camera;
int lastX = 0;
int lastY = 0;

// Entry
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	HWND appWnd = initWindow(hInstance);
	MSG msg = {0};
	Rasterizer rasterizer;
	PerformanceClock gameClock;
	gameClock.reset();

	//TEMP
	camera.setView(XMVectorSet(0.f, 0.f, 5.f, 1.f), XMVectorSet(0.f, 0.f, -1.f, 1.f), XMVectorSet(0.f, 1.f, 0.f, 0.f));
	camera.setProj(XM_PI * 0.45f, 800.f/640.f, 0.1f, 800.f);
	//END TEMP

	if (appWnd) {
		if (FAILED(initApp(appWnd, rasterizer, nCmdShow)))
			return -1;

		while (WM_QUIT != msg.message) {
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				gameClock.tick();
				camera.update(gameClock.getDeltaTime());
				rasterizer.setMatrices(XMMatrixIdentity(), camera.getViewMatrix(), camera.getProjMatrix());
				if(FAILED(rasterizer.render()))
					return -1;
			}
		}
	}

	DestroyWindow(appWnd);
	return (int) msg.wParam;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
				PostQuitMessage(0);

			Input::onKeyDown(wParam);

			return 0;

		case WM_KEYUP:
			Input::onKeyUp(wParam);
			return 0;

		case WM_LBUTTONDOWN:
			lastX = GET_X_LPARAM(lParam);
			lastY = GET_Y_LPARAM(lParam);

			SetCapture(hWnd);

			return 0;

		case WM_LBUTTONUP:
			ReleaseCapture();
			return 0;

		case WM_MOUSEMOVE:
			camera.onMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), lastX, lastY);

			lastX = GET_X_LPARAM(lParam);
			lastY = GET_Y_LPARAM(lParam);
			
			return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND initWindow(HINSTANCE hInstance) {
	// Describe a window class
	WNDCLASSEX wndClassEx;
	ZeroMemory(&wndClassEx, sizeof(wndClassEx));

	wndClassEx.cbSize = sizeof(WNDCLASSEX);
	wndClassEx.style = CS_HREDRAW | CS_VREDRAW;
	wndClassEx.lpfnWndProc = wndProc;
	wndClassEx.hInstance = hInstance;
	wndClassEx.lpszClassName = L"mainWndClass";

	// Try to register the wnd class.
	if (!RegisterClassEx(&wndClassEx))
		return false;

	// Adjust size of window
	RECT clientRect = {0, 0, 800, 640};
	AdjustWindowRect(&clientRect, WS_OVERLAPPEDWINDOW, FALSE);

	// Create the window.
	HWND hWnd = CreateWindow(
		L"mainWndClass",
		L"BTH Direct3D Project",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		clientRect.right  - clientRect.left,
		clientRect.bottom - clientRect.top,
		nullptr,
		nullptr,
		hInstance,
		nullptr
	);

	return hWnd;
}

HRESULT initApp(HWND hWnd, Rasterizer& rasterizer, int nCmdShow) {
	HRESULT hr;
	if (FAILED(hr = rasterizer.initDirect3D(hWnd, 800, 640)))
		return hr;

	ShowWindow(hWnd, nCmdShow);

	return hr;
}