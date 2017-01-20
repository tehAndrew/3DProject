#include "Input.h"

unsigned int Input::keyCodes[4] = {0x57, 0x41, 0x53, 0x44};
short        Input::keyState[4] = {0};

bool Input::keyPressed(Keys key) {
	if (key == UNKNOWN_KEY)
		return false;

	return keyState[key] == PRESSED;
}

bool Input::keyHold(Keys key) {
	if (key == UNKNOWN_KEY)
		return false;

	return keyState[key] == HOLD || keyState[key] == PRESSED;
}

void Input::onKeyDown(WPARAM keyCode) {
	Keys key = findKey(keyCode);

	if (key == UNKNOWN_KEY)
		return;

	if (keyState[key] = WAITING)
		keyState[key] = PRESSED;
	if (keyState[key] = PRESSED)
		keyState[key] = HOLD;
}

void Input::onKeyUp(WPARAM keyCode) {
	Keys key = findKey(keyCode);

	if (key == UNKNOWN_KEY)
		return;

	keyState[key] = WAITING;
}

Input::Keys Input::findKey(WPARAM keyCode) {
	Keys key;

	for (int i = 0; i < amountKeys; i++) {
		if (keyCodes[i] == keyCode) {
			key = Keys(i);
			return key;
		}
	}

	return UNKNOWN_KEY;
}