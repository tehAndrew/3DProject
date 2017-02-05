#ifndef INPUT_H
#define INPUT_H

#include <windows.h>

class Input {
	public:
		// UNKNOWN_KEY is any key thats not predefined.
		enum Keys {
			Q_KEY, W_KEY, E_KEY, A_KEY, S_KEY, D_KEY, UNKNOWN_KEY
		};

		enum KeyState {
			WAITING, PRESSED, HOLD
		};

		static bool keyPressed(Keys key);
		static bool keyHold(Keys key);

		static void onKeyDown(WPARAM keyCode);
		static void processPressedKeys();
		static void onKeyUp(WPARAM keyCode);

	private:
		Input()  {};
		~Input() {};

		static const unsigned short mAmountKeys = 6;

		static unsigned int mKeyCodes[];
		static short		mKeyState[];

		static Keys findKey(WPARAM keyCode);
};

#endif