#include <windowsx.h>
#include <cstdlib>
#include <ctime>

#include "Input.h"
#include "World3D.h"
#include "PerformanceClock.h"

// Forward declaration
LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
HWND initWindow(HINSTANCE hInstance);
HRESULT initApp(HWND hWnd, int nCmdShow);

// Some globals
HWND appWnd;
World3D world;
PerformanceClock gameClock;

// For calculating mouse distance moved
int mouseLastX = 0;
int mouseLastY = 0;

// The starting client size of this app
int clientWidth  = 1280;
int clientHeight = 800;

// Entry
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow) {
	MSG msg = { 0 };
	
	// Init rng
	std::srand(std::time(NULL));

	appWnd = initWindow(hInstance);
	gameClock.reset();

	if (appWnd) {
		if (FAILED(initApp(appWnd, nCmdShow)))
			return -1;

		// GAME LOOP
		while (WM_QUIT != msg.message) {
			// Message loop
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				// Handle messages if there are any.
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			// Game related updating
			else {
				// Tick game clock to be able see how much time passed since last tick.
				gameClock.tick();

				// Update and render world
				world.update(gameClock.getDeltaTime());
				if(FAILED(world.render()))
					return -1;

				// Change pressed keys to held down keys. Necessary because of how the message handle works.
				Input::processPressedKeys();
			}
		}
	}

	DestroyWindow(appWnd);
	return (int) msg.wParam;
}

LRESULT CALLBACK wndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		// Exit when the window is destroyed.
		case WM_DESTROY:
			PostQuitMessage(0);
			return 0;

		// Register key press to the Input class.
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE)
				PostQuitMessage(0);

			Input::onKeyDown(wParam);

			return 0;

		// Register key up to the Input class.
		case WM_KEYUP:
			Input::onKeyUp(wParam);
			return 0;

		// Capture mouse to our window when left button is down, every drag registered during mouse capture will produce messages 
		// regardless where the mouse cursor is on the screen.
		case WM_LBUTTONDOWN:
			mouseLastX = GET_X_LPARAM(lParam);
			mouseLastY = GET_Y_LPARAM(lParam);

			SetCapture(hWnd);

			return 0;

		// Free the mouse capture so the mouse can be used outside of the application.
		case WM_LBUTTONUP:
			ReleaseCapture();
			return 0;

		// When mouse is captured, register distance the mouse moves.
		case WM_MOUSEMOVE:
			world.onMouseMove(wParam, GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam), mouseLastX, mouseLastY);

			mouseLastX = GET_X_LPARAM(lParam);
			mouseLastY = GET_Y_LPARAM(lParam);
			
			return 0;
	}

	return DefWindowProc(hWnd, message, wParam, lParam);
}

// Initializes out one and only window.
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
	RECT clientRect = {0, 0, clientWidth, clientHeight};
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

// Inits all of the app.
HRESULT initApp(HWND hWnd, int nCmdShow) {
	HRESULT hr;
	if (FAILED(hr = world.init(hWnd, clientWidth, clientHeight, 30)))
		return hr;

	ShowWindow(hWnd, nCmdShow);

	return hr;
}