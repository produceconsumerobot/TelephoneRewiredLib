//
//  experimentGoverer.h
//
//  Created by Sean Montgomery on 12/18/12.
//  http://produceconsumerobot.com/
//
//  This work is licensed under the Creative Commons 
//  Attribution-ShareAlike 3.0 Unported License. 
//  To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/.
//

#include<stdio.h>
#include<cstdlib>
#include<iostream>
#include<string.h>
#include<fstream>
//#include<dirent.h>

#include <vector>
#include <algorithm> // includes max function
#include "ofMain.h"
#include "myUtils.h"

#ifndef _EXPERIMENT_GOVERNER
#define _EXPERIMENT_GOVERNER


class Stimulus {
public:
	enum types {Text, Sound, None};
	Stimulus();
	Stimulus(types type, string data);
	void playStimulus();
	void stopStimulus();
	bool isPlaying();
	string str();

private:
	string _data;
	types _type;
	ofColor _fontColor;
	ofTrueTypeFont _font;
	ofPoint _stimulusCenter;
	bool _isPlaying;
	ofSoundPlayer _mySound;

	void _setup();
};

class StimulusPlayer {
private:
	std::vector<Stimulus> _allStimuli;
	std::vector<Stimulus> _stimulusCycle;
	//Stimulus _currentStimulus;
	bool _stimulusCycleOn;

	// Stimulus timing varaiables (in milliseconds)
	unsigned long _currentStimulusTimerStart;
	unsigned long _stimulusOnTime;
	unsigned long _currentStimulusDelayTime;
	unsigned long _interStimulusBaseDelayTime;
	unsigned long _interStimulusRandDelayTime;
	//unsigned long _initialStimulusDelay;

	int _stimulusIterator;
	int _nStimuliToShow;

	// Stimuli properties
	ofColor _fontColor;
	//ofPoint _stimulusCenter;

	void _setup();

	string _textFilePath;
	string _soundDirPath;

	void setTextFilePath(string textFilePath);
	void setSoundDirPath(string soundDirPath);

	bool isQueuedStart;

	bool _iterateOnButtonPress;
	bool _iterateOnTimer;


public:
	/* 
	** StimulusPlayer()
	**		Sets random seed?
	**		Set stimulus iterator to number of stimuli to show
	*/
	StimulusPlayer();

	// StimulusPlayer(string path)
	//		Loads stimuli from given path
	StimulusPlayer(string path); 

	/* 
	** loadStimuli(string path)
	**		Loads stimuli from path
	*/
	int loadStimuli(string textFilePath, string soundDirPath);
	int loadStimuli();

	/*
	** void setTimes(unsigned long baseOnTime, unsigned long randOnTime, unsigned long interStimulusDelay)
	**		Sets stimulus timing (in milliseconds)
	*/
	void setTimes(float stimulusOnTime, float interStimulusBaseDelayTime, float interStimulusRandDelayTime);

	void setIterators(bool iterateOnTimer, bool iterateOnButtonPress) ;

	void buttonPressed();
	void nextStimulus();

	/* 
	** startStimulusCycle()
	**		Starts the stimulus cycle
	**	ALGORITHM:
	**		Sets timers
	**		Flips cycleOn bit
	**		Resets the stimulus iterator
	**		Recalculate the stimulus ON time (with randomness)
	*/
	void start();

	/* 
	** queueStart()
	**	ALGORITHM:
	**		Sets a flag so that on next update, stimuli are loaded and started
	**		This is a bit of a hack to overcome slowness of file loadStimuli 
	**		and avoid showing an old screen during this period.
	**		Might be better done with a separate thread.
	*/
	void queueStart(string textFilePath, string soundDirPath);

	void randomizeStimuli(); // Randomizes the stimulus order

	/* 
	** updateStimulus()
	**	ALGORITHM:
	**		Checks current time against stimulus times 
	**		Shows the stimulus if it's time 
	**		Updates the stimulus iterator
	**	OUTPUT:
	**		int nRemaining	-- number of remaining stimuli to show
	*/
	int update();

	//Stimulus getCurrentStimulus();

	ofEvent<Stimulus> stimulusPlay;
	ofEvent<Stimulus> stimulusStop;
};

class InstructionsPlayer 
{

private:
	int _nPages;
	int _currentPage;

	float _timeoutDelay; // Seconds
	unsigned long _lastButtonPressTime;
	unsigned long _participantCode;

public:
	//InstructionsPlayer();
	InstructionsPlayer(int nPages=0, float timeout=3.);

	/* 
	** update()
	**	ALGORITHM:
	**		shows the appropriate text for the current page
	**		checks for timeout since last button press.
	*/
	void update();

	/* 
	** buttonPressed()
	**	ALGORITHM:
	**		updates _lastButtonPressTime
	**		initiates page change
	*/
	void buttonPressed();
	void showPage(int i);
	void goToPage(int i);
	int remaining();
	void setParticipantCode(unsigned long participantCode);
	int getPageNum();

	ofEvent<int> newPage;
	ofEvent<int> drawPage;
};

class TimedPagePlayer 
{
public:
	static enum pages {Congratulations, ThankYou, BlankPage};

	TimedPagePlayer();
	void start(float duration=1., pages page=TimedPagePlayer::BlankPage);
	bool update();
	void setParticipantCode(unsigned long participantCode);

	ofEvent<int> drawPage;

private:
	unsigned long _startTime;
	pages _currentPage;
	//int _pageNumber;
	float _onDuration;
	unsigned long _participantCode;

};


// Directs traffic of Experiment, what to show, when
class ExperimentGovernor 
{

public:
	static enum states {Instructions, BlankPage, StimulusPresentation, Congratulations, None}; // None must be last!!
	static string getStateString(states state);	vector<bool> enabledStates;

	ExperimentGovernor();
	//ExperimentGovernor(InstructionsPlayer ip, StimulusPlayer sp);

	void update(); // Controls instructions/stimulus presentation
	void buttonPressed(); // Input detected
	states getState();
	void setInstructionsPlayer(InstructionsPlayer * p);
	void setStimulusPlayer(StimulusPlayer * p);
	void setTimedPagePlayer(TimedPagePlayer * p);

	void nextState();
	void goToState(states state);
	unsigned long generateParticipantID(unsigned long participantNumber);
	unsigned long reverseParticipantID(unsigned long participantID);
	unsigned long getParticipantID();

	ofEvent<string> newState;
	ofEvent<unsigned long> newParticipant;

	void setTimeoutDelay(float timeoutDelay);

	void setCongratulationsTime(float congratulationsTime);

	void addStimulusPaths(std::vector< string >  stimulusDirectories);
	void addStimulusPaths(string textFilePath, string soundDirPath );
	void setStimulusPaths(std::vector< std::vector< string > > stimulusDirectories);

	//void includeInstructions(bool b);
	//void includeStimuli(bool b);
private:
	states _currentState;
	InstructionsPlayer * _instructionsPlayer;
	StimulusPlayer * _stimulusPlayer;
	TimedPagePlayer * _timedPagePlayer;
	unsigned long _participantNumber;
	unsigned long _lastButtonDownTime;
	float _congratulationsTime;
	float _timeoutDelay;
	std::vector< std::vector< string > > _stimulusPaths;

};


#endif