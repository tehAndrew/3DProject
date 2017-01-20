#ifndef INPUT_H
#define INPUT_H

#include <windows.h>

class Input {
	public:
		enum Keys {
			W_KEY, A_KEY, S_KEY, D_KEY, UNKNOWN_KEY
		};

		enum KeyState {
			WAITING, PRESSED, HOLD
		};

		static bool keyPressed(Keys key);
		static bool keyHold(Keys key);

		static void onKeyDown(WPARAM keyCode);
		static void onKeyUp(WPARAM keyCode);

		static void resetMousePos();
		static void registerMousePos();
		static int getMouseDeltaX();
		static int getMouseDeltaY();

	private:
		Input()  {};
		~Input() {};

		static const unsigned short amountKeys = 4;

		static int mousePosX;
		static int mousePosY;

		static unsigned int keyCodes[];
		static short keyState[];

		static Keys findKey(WPARAM keyCode);
};

#endif