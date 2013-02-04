//
//  myUtils.h
//
//  Created by Sean Montgomery on 12/18/12.
//  http://produceconsumerobot.com/
//
//  This work is licensed under the Creative Commons 
//  Attribution-ShareAlike 3.0 Unported License. 
//  To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/.
//


#ifndef _MY_UTILS
#define _MY_UTILS

#include "ofMain.h"

static string strVectorF(std::vector<float> vec)
{
	std::ostringstream os;
	for (int i=0; i<vec.size(); i++) {
		//s = s + vec.at(i) + ",";
		os << fixed << setprecision(3)  << vec.at(i) << ",";
	}
	return os.str();
};

static unsigned long myStartTime;

static unsigned long myGetSystemTime( ) {
	return (unsigned long) ofGetSystemTime();
};

static unsigned long myGetElapsedTimeMillis() {
	return myGetSystemTime() - myStartTime;
};

static float myGetElapsedTimef(){
	return myGetElapsedTimeMillis() / 1000.0f;
};

static void myResetElapsedTimeCounter(){
	myStartTime = (unsigned long) myGetSystemTime();
};

class KeyStateTracker 
{
public:
	struct KeyState 
	{
		char key;
		bool state;
	} ;

	KeyStateTracker();

	bool getKeyState(char key);

	void setKeyState(char key, bool state);

	void toggleKeyState(char key);

private:
	std::vector<KeyState> _keyPressedStates;

	void addKey(char key, bool state);
};

#endif
