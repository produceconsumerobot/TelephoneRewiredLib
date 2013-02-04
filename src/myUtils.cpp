//
//  myUtils.cpp
//
//  Created by Sean Montgomery on 12/18/12.
//  http://produceconsumerobot.com/
//
//  This work is licensed under the Creative Commons 
//  Attribution-ShareAlike 3.0 Unported License. 
//  To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/.
//

#include "myUtils.h"

KeyStateTracker::KeyStateTracker() {
}

bool KeyStateTracker::getKeyState(char key) {
	for (int i=0; i<_keyPressedStates.size(); i++) {
		if (key == _keyPressedStates.at(i).key) {
			return _keyPressedStates.at(i).state;
		}
	}
	return false;
}

void KeyStateTracker::setKeyState(char key, bool state) {
	for (int i=0; i<_keyPressedStates.size(); i++) {
		if (key == _keyPressedStates.at(i).key) {
			_keyPressedStates.at(i).state = state;
			return;
		}
	}
	addKey(key, state);
	return;
}

void KeyStateTracker::toggleKeyState(char key) {
	if (getKeyState(key)) {
		setKeyState(key, false);
	} else {
		setKeyState(key, true);
	}
}

void KeyStateTracker::addKey(char key, bool state) {
	KeyState kps;
	kps.key = key;
	kps.state = state;
	_keyPressedStates.push_back(kps);
}
