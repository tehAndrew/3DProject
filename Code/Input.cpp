#include "Input.h"

// Predefined keys.
unsigned int Input::mKeyCodes[] = { 0x51, 0x57, 0x45, 0x41, 0x53, 0x44};
short        Input::mKeyState[mAmountKeys] = { WAITING };

// Returns whether a predefined key is pressed or not.
bool Input::keyPressed(Keys key) {
	if (key == UNKNOWN_KEY)
		return false;

	return mKeyState[key] == PRESSED;
}

// Returns whether a predefined key is held down or not.
bool Input::keyHold(Keys key) {
	if (key == UNKNOWN_KEY)
		return false;

	return mKeyState[key] == HOLD;
}

// Used in the window procedure. It changes state of pressed keys.
void Input::onKeyDown(WPARAM keyCode) {
	Keys key = findKey(keyCode);

	if (key == UNKNOWN_KEY)
		return;

	if      (mKeyState[key] == WAITING)
		mKeyState[key] = PRESSED;
}

// Due to the nature of the key down message it is needed to change the pressed state to hold in the game loop.
// Put this after everything else is processed in the game loop.
void Input::processPressedKeys() {
	for (int i = 0; i < mAmountKeys; i++) {
		if (mKeyState[i] == PRESSED)
			mKeyState[i] = HOLD;
	}
}

// Used in the window procedure. It changes state of pressed keys.
void Input::onKeyUp(WPARAM keyCode) {
	Keys key = findKey(keyCode);

	if (key == UNKNOWN_KEY)
		return;

	mKeyState[key] = WAITING;
}

// A helper function that translates a key code into an array index.
Input::Keys Input::findKey(WPARAM keyCode) {
	Keys key;

	for (int i = 0; i < mAmountKeys; i++) {
		if (mKeyCodes[i] == keyCode) {
			key = Keys(i);
			return key;
		}
	}

	return UNKNOWN_KEY;
}