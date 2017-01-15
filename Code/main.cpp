#include <windows.h>

#include "Rasterizer.h"

// Forward declaration
LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND initWindow(HINSTANCE hInstance);
HRESULT initApp(HWND hWnd, Rasterizer& rasterizer, int nCmdShow);

// Entry
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	HWND appWnd = initWindow(hInstance);
	MSG msg = {0};
	Rasterizer rasterizer;

	if (appWnd) {
		if (FAILED(initApp(appWnd, rasterizer, nCmdShow)))
			return -1;

		while (WM_QUIT != msg.message) {
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				// Game Loop
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
			break;
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
	RECT clientRect = {0, 0, 600, 400};
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
	if (FAILED(hr = rasterizer.initDirect3D(hWnd, 600, 400)))
		return hr;

	ShowWindow(hWnd, nCmdShow);

	return hr;
}