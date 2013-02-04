//
//  experimentGovernor.cpp
//
//  Created by Sean Montgomery on 12/18/12.
//  http://produceconsumerobot.com/
//
//  This work is licensed under the Creative Commons 
//  Attribution-ShareAlike 3.0 Unported License. 
//  To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/3.0/.
//

#include "experimentGovernor.h"


Stimulus::Stimulus() 
{
	_type = None;
	_data = "";

	_setup();
}

Stimulus::Stimulus(types type, string data) 
{
	_type = type;
	_data = data;

	_setup();
}

void Stimulus::_setup() 
{
	_font.loadFont("verdana.ttf", 80, true, true);
	_fontColor = ofColor(255,255,255);
	_stimulusCenter = ofPoint(ofGetWindowWidth()/2, ofGetWindowHeight()/2);
	_isPlaying = false;
	//std::stringstream ss;
	cout << Stimulus::Sound;
	//cout << ss;
}

void Stimulus::playStimulus() 
{
	ofRectangle bounds;

	switch (_type) {
	case Sound:
		// play sound
		if (!_isPlaying) {
			//ofSoundPlayer _mySound;
			_mySound.loadSound(_data); //DOCUMENTATION PAGE EXAMPLE IS WRONG
			_mySound.setVolume(0.05f);
			_mySound.play();
		}

		//cout << "playing sound \n";
		break;
	case Text:
		// show text

		ofPushMatrix();
		ofPushStyle();
		bounds = _font.getStringBoundingBox(_data, 0, 0);
		ofTranslate(-bounds.width/2, bounds.height / 2, 0);
		ofSetColor(_fontColor);
		_font.drawString(_data, _stimulusCenter.x, _stimulusCenter.y);
		ofPopStyle();
		ofPopMatrix();

		//cout << "showing text \n";
		break;
	default:
		break;
	}

	_isPlaying = true;
}

void Stimulus::stopStimulus() 
{
	_isPlaying = false;
}

bool Stimulus::isPlaying()
{
	return _isPlaying;
}

string Stimulus::str()
{
	string s;
	switch(_type) 
	{
	case Stimulus::Sound:
		s = "Sound";
		break;
	case Stimulus::Text:
		s = "Text";
		break;
	default:
		s = "None";
	}
	return s + "," + _data;
}


StimulusPlayer::StimulusPlayer() {
	//loadStimuli("/stimuli/");
	//_nStimuliToShow = min(15, (int) _stimulusCycle.size());

	_setup();
}

/*
StimulusPlayer::StimulusPlayer(string path) {
	//string path = "data/stimuli/";
	//loadStimuli(path);
	//_nStimuliToShow = min(15, (int) _stimulusCycle.size());

	_setup();
}
*/

void StimulusPlayer::_setup() 
{
	ofSeedRandom();
	_nStimuliToShow = 0;
	_stimulusIterator = _nStimuliToShow;
	_stimulusCycleOn = false;

	_textFilePath = "";
	_soundDirPath = "";

	isQueuedStart = false;
	_iterateOnButtonPress = false;
	_iterateOnTimer = true;
	//_stimulusCenter = ofPoint(ofGetWindowWidth()/2, ofGetWindowHeight()/2);
}

int StimulusPlayer::loadStimuli(string textFilePath, string soundDirPath) 
{
	//cout << "StimulusPlayer::loadStimuli  " << myGetElapsedTimef() << "\n";

	// Load sounds
	std::vector<Stimulus> sounds;
	//string t = ofToDataPath(soundDirPath);
	ofDirectory dir1(ofToDataPath(soundDirPath)); // REPORT BUG
	ofDirectory dir(soundDirPath);
	bool b = dir.exists();
	string s = dir.getAbsolutePath();
	bool b1 = dir1.exists();
	string s1 = dir1.getAbsolutePath();
	//cout << "StimulusPlayer::loadStimuli dir " << myGetElapsedTimef() << "\n";
	if (dir.exists()) {
		//cout << "StimulusPlayer::loadStimuli dir.exists() " << myGetElapsedTimef() << "\n";
		dir.allowExt("mp3");
		//cout << "StimulusPlayer::loadStimuli dir.allowExt(); " << myGetElapsedTimef() << "\n";
		dir.listDir();
		//cout << "StimulusPlayer::loadStimuli dir.listDir(); " << myGetElapsedTimef() << "\n";
		int n = dir.numFiles();
		//cout << "StimulusPlayer::loadStimuli dir.numFiles(); " << myGetElapsedTimef() << "\n";
		for (int i=0; i<n; i++) {
			sounds.push_back(Stimulus(Stimulus::Sound, dir.getPath(i)));
			//cout << "StimulusPlayer::loadStimuli dir.getPath " << myGetElapsedTimef() << "\n";
		}
	}
	//cout << "StimulusPlayer::loadStimuli sounds loaded " << myGetElapsedTimef() << "\n";
	dir.close();

	// Load Text
	std::vector<Stimulus> text;
	ofFile file(ofToDataPath(textFilePath));
	if (file.exists() && file.canRead()) {
		ofBuffer buffer = file.readToBuffer();
		while (!buffer.isLastLine()) {
			text.push_back(Stimulus(Stimulus::Text, buffer.getNextLine()));
		}
	}
	//cout << "StimulusPlayer::loadStimuli text loaded " << myGetElapsedTimef() << "\n";
	file.close();

	_allStimuli.clear();
	//cout << "StimulusPlayer::loadStimuli allStimuli cleared " << myGetElapsedTimef() << "\n";
	// Create stimulus vector by interleaving sounds and text
	if (text.size() != sounds.size()) {
		fprintf(stderr, "Error StimulusPlayer::loadStimuli -- text.size() != sounds.size()");
		return -1;
	} else {
		for (int i=0; i<sounds.size(); i++) {
			_allStimuli.push_back(sounds.at(i));
			_allStimuli.push_back(text.at(i));
		}
	}
	//cout << "StimulusPlayer::loadStimuli allStimuli pushed " << myGetElapsedTimef() << "\n";

	_stimulusCycle = _allStimuli;
	_nStimuliToShow = _stimulusCycle.size();
	_stimulusIterator = _nStimuliToShow;

	//cout << "END StimulusPlayer::loadStimuli " << myGetElapsedTimef() << "\n";
	return 0;
}

void StimulusPlayer::setTimes(float stimulusOnTime, float interStimulusBaseDelayTime, float interStimulusRandDelayTime) 
{
	_stimulusOnTime = stimulusOnTime * 1000;
	_interStimulusBaseDelayTime = interStimulusBaseDelayTime * 1000;
	_interStimulusRandDelayTime = interStimulusRandDelayTime * 1000;
}

void StimulusPlayer::setIterators(bool iterateOnTimer, bool iterateOnButtonPress) 
{ 
	_iterateOnButtonPress = iterateOnButtonPress;
	_iterateOnTimer = iterateOnTimer;
}


void StimulusPlayer::start() {
	_stimulusCycleOn = true;
	_stimulusIterator = 0;
	_currentStimulusDelayTime = _interStimulusBaseDelayTime + ((unsigned long) ofRandom(0, _interStimulusRandDelayTime));
	_currentStimulusTimerStart = myGetElapsedTimeMillis();
}

void StimulusPlayer::queueStart(string textFilePath, string soundDirPath) {
	isQueuedStart = true;
	setTextFilePath(textFilePath);
	setSoundDirPath(soundDirPath);
}

void StimulusPlayer::setTextFilePath(string textFilePath) 
{
	_textFilePath = textFilePath;
}

void StimulusPlayer::setSoundDirPath(string soundDirPath)
{
	_soundDirPath = soundDirPath;
}

void StimulusPlayer::randomizeStimuli()
{
	_stimulusCycle = _allStimuli;
	ofRandomize(_stimulusCycle);
	_stimulusCycle.erase(_stimulusCycle.begin(), _stimulusCycle.begin() + _nStimuliToShow);
}

void StimulusPlayer::buttonPressed() 
{
	if (_iterateOnButtonPress) {
		nextStimulus();
	}
}

void StimulusPlayer::nextStimulus() {
	// Stop current stimulus
	_stimulusCycle.at(_stimulusIterator).stopStimulus();
	// Callback to main code for logging
	Stimulus s = _stimulusCycle.at(_stimulusIterator);
	ofNotifyEvent(stimulusStop, s, this);

	// iterate stimulus
	_stimulusIterator++;

	// Recalculate the stimulus ON time (with randomness)
	_currentStimulusDelayTime = _interStimulusBaseDelayTime + ((unsigned long) ofRandom(0, _interStimulusRandDelayTime));
	_currentStimulusTimerStart = myGetElapsedTimeMillis();

	// If we reached the number of stimuli to show
	if (_stimulusIterator >= _nStimuliToShow) {
		// Turn the cycle off
		_stimulusCycleOn = false;
	}
}

int StimulusPlayer::update() {
	//cout << "StimulusPlayer::updateStimulus()\n";

	if (isQueuedStart) {

		isQueuedStart = false;

		// Load the appropriate stimuli
		loadStimuli(_textFilePath, _soundDirPath);
		//Start the Stimulus Player
		start();

	}

	if (_stimulusCycleOn) {

		if (_stimulusIterator >= _nStimuliToShow) {
			return _nStimuliToShow - _stimulusIterator;
		}

		unsigned long currentTime = myGetElapsedTimeMillis();

		// If interstimulus delay exceeded
		if ((currentTime - _currentStimulusTimerStart) > _currentStimulusDelayTime) {

			// If stimulus ON interval exceeded
			if ((currentTime - _currentStimulusTimerStart) 
		> (_currentStimulusDelayTime + _stimulusOnTime)) 
			{

				if (_iterateOnTimer) {
					nextStimulus();
				}

			} else { // If interstimulus delay NOT exceeded

				// If we we're not currently showing a stimlus
				if (!_stimulusCycle.at(_stimulusIterator).isPlaying()) {
					// We're turning on a stimulus
					// Callback to main code for logging
					Stimulus s = _stimulusCycle.at(_stimulusIterator);
					ofNotifyEvent(stimulusPlay, s, this);
				}

				// Show stimulus
				_stimulusCycle.at(_stimulusIterator).playStimulus();
			}
		}

		return _nStimuliToShow - _stimulusIterator;
	}
	return 0;
}

/*
InstructionsPlayer::InstructionsPlayer() 
{
_nPages = 3;
_timeoutDelay = 3.;
}
*/

InstructionsPlayer::InstructionsPlayer(int nPages, float timeout) 
{
	_nPages = nPages;
	_currentPage = 0;
	_timeoutDelay = timeout;
	_lastButtonPressTime = myGetElapsedTimeMillis();
	setParticipantCode(777777); 
}

void InstructionsPlayer::update()
{
	if ((myGetElapsedTimeMillis() - _lastButtonPressTime) < ((unsigned long) (_timeoutDelay*1000))) {
		showPage(_currentPage);
	} else {
		goToPage(0);
	}
}

void InstructionsPlayer::buttonPressed()
{
	_lastButtonPressTime = myGetElapsedTimeMillis();
	if (_currentPage + 1 < _nPages) {
		goToPage(_currentPage + 1);
	} 
}

void InstructionsPlayer::goToPage(int i)
{
	if (i != _currentPage) {
		_currentPage = i;
		ofNotifyEvent(newPage, i, this);
	}
	if (_currentPage < 2) {
		setParticipantCode(777777);
	}
	showPage(_currentPage);
}

int InstructionsPlayer::remaining() 
{
	return _nPages - _currentPage -1;
}


void InstructionsPlayer::setParticipantCode(unsigned long participantCode) 
{
	_participantCode = participantCode;
}

void InstructionsPlayer::showPage(int i)
{
	ofNotifyEvent(drawPage, i, this);
}

ExperimentGovernor::ExperimentGovernor() 
{
	_currentState = ExperimentGovernor::None;
	_participantNumber = 0;
	_congratulationsTime = 15;
	_timeoutDelay = -1;
	_lastButtonDownTime = myGetElapsedTimeMillis();

	enabledStates.resize(ExperimentGovernor::None + 1); // only works if None is last!!
	enabledStates.at(ExperimentGovernor::Instructions) = false;
	enabledStates.at(ExperimentGovernor::BlankPage) = false;
	enabledStates.at(ExperimentGovernor::StimulusPresentation) = false;
	enabledStates.at(ExperimentGovernor::Congratulations) = false;
	enabledStates.at(ExperimentGovernor::None) = true;
}

void ExperimentGovernor::setTimeoutDelay(float timeoutDelay) {
	_timeoutDelay = timeoutDelay;
}

/*
ExperimentGovernor::ExperimentGovernor(InstructionsPlayer ip, StimulusPlayer sp)
{
_participantNumber = 0;

setInstructionsPlayer(ip);
setStimulusPlayer(sp);

enabledStates.resize(ExperimentGovernor::None + 1); // only works if None is last!!
enabledStates.at(ExperimentGovernor::None) = true;

// Go to next state
_currentState = ExperimentGovernor::Instructions;
}
*/


void ExperimentGovernor::addStimulusPaths(std::vector< string > stimulusPaths)
{
	if (_stimulusPaths.size() == 0) {
		_stimulusPaths.push_back(stimulusPaths);
	} else if ( _stimulusPaths.at(0).size() == stimulusPaths.size()) {
		_stimulusPaths.push_back(stimulusPaths);
	} else {
		fprintf(stderr, "Error in ExperimentGovernor::addStimulusDirectories: _stimulusDirectories.at(0).size() != stimulusDirectories.size()");
	}
}

void ExperimentGovernor::addStimulusPaths(string textFilePath, string soundDirPath )
{
	std::vector< string > dirs;
	dirs.push_back(textFilePath);
	dirs.push_back(soundDirPath);
	addStimulusPaths(dirs);
}

void ExperimentGovernor::setStimulusPaths(std::vector< std::vector< string > > stimulusPaths)
{
	stimulusPaths = stimulusPaths;
}



void ExperimentGovernor::setInstructionsPlayer(InstructionsPlayer * ip) 
{
	_instructionsPlayer = ip;
	enabledStates.at(ExperimentGovernor::Instructions) = true;
}

void ExperimentGovernor::setStimulusPlayer(StimulusPlayer * sp) 
{
	_stimulusPlayer = sp;
	enabledStates.at(ExperimentGovernor::StimulusPresentation) = true;
	enabledStates.at(ExperimentGovernor::BlankPage) = true;
	enabledStates.at(ExperimentGovernor::Congratulations) = true;
}

void ExperimentGovernor::setTimedPagePlayer(TimedPagePlayer * tp) 
{
	_timedPagePlayer = tp;
}

void ExperimentGovernor::goToState(states state)
{
	switch (state) {
	case ExperimentGovernor::Instructions:
		{
			_currentState = ExperimentGovernor::Instructions;
			string s = getStateString(_currentState);
			ofNotifyEvent(newState, s, this);
			_instructionsPlayer->goToPage(0);
		}
		break;

	case ExperimentGovernor::BlankPage:
		{
			_currentState = ExperimentGovernor::BlankPage;
			string s = getStateString(_currentState);
			ofNotifyEvent(newState, s, this);
			_timedPagePlayer->start(0.5);
		}
		break;

	case ExperimentGovernor::StimulusPresentation:
		{
			_currentState = ExperimentGovernor::StimulusPresentation;
			string s = getStateString(_currentState);
			ofNotifyEvent(newState, s, this);
			
			// If we have stimulusDirectories set and they have two columns
			if ((_stimulusPaths.size() > 0) && (_stimulusPaths.at(0).size() ==2)) {
			_stimulusPlayer->queueStart(
				_stimulusPaths.at(_participantNumber % _stimulusPaths.size()).at(0), 
				_stimulusPaths.at(_participantNumber % _stimulusPaths.size()).at(1));
			} else {
				fprintf(stderr, "Error ExperimentGovernor::goToState: _stimulusDirectories wrong size");
			}
			
			/*
			if (_participantNumber % 2) { 
				_stimulusPlayer->queueStart("data/stimuli/training/text/form4.txt", "stimuli/training/audio/form1/");
			} else {
				_stimulusPlayer->queueStart("data/stimuli/training/text/form1.txt", "stimuli/training/audio/form4/");
			}
			*/
			/*
			// Load the appropriate stimuli
			if (_participantNumber % 2) { 
			_stimulusPlayer->loadStimuli("data/stimuli/text/form4.txt", "stimuli/audio/form1/");
			} else {
			_stimulusPlayer->loadStimuli("data/stimuli/text/form1.txt", "stimuli/audio/form4/");
			}
			//Start the Stimulus Player
			_stimulusPlayer->start();
			*/
		}
		break;
	case ExperimentGovernor::Congratulations:
		{
			_currentState = ExperimentGovernor::Congratulations;
			string s = getStateString(_currentState);
			ofNotifyEvent(newState, s, this);
			_timedPagePlayer->start(_congratulationsTime, TimedPagePlayer::Congratulations);
		}
		break;
	case ExperimentGovernor::None:
		{
			_currentState = ExperimentGovernor::None;
			string s = "None";
			ofNotifyEvent(newState, s, this);
		}
		break;
	default:
		break;
	}
	update();
}

string ExperimentGovernor::getStateString(states state) 
{
	switch (state) {
	case Instructions:
		return "Instructions";
		break;
	case BlankPage:
		return "BlankPage";
		break;
	case StimulusPresentation:
		return "StimulusPresentation";
		break;
	case Congratulations:
		return "Congratulations";
		break;
	case None:
		return "None";
		break;
	default:
		return "";
		break;
	}
}

void ExperimentGovernor::nextState()
{
	switch (_currentState) {

		// Instructions
	case ExperimentGovernor::Instructions: 
		if (enabledStates.at(ExperimentGovernor::BlankPage)) {
			// Go to BlankPage
			goToState(ExperimentGovernor::BlankPage);

		} else if (enabledStates.at(ExperimentGovernor::StimulusPresentation)) {
			// Go to StimulusPresentation
			goToState(ExperimentGovernor::StimulusPresentation);

		} else if (enabledStates.at(ExperimentGovernor::Instructions) == true) {
			// Go to Instructions
			goToState(ExperimentGovernor::Instructions);

		} else {
			// Go to None
			goToState(ExperimentGovernor::None);
		}
		break;

		// BlankPage
	case ExperimentGovernor::BlankPage: 
		if (enabledStates.at(ExperimentGovernor::StimulusPresentation)) {
			// Go to StimulusPresentation
			goToState(ExperimentGovernor::StimulusPresentation);
		} else {
			// Go to None
			goToState(ExperimentGovernor::None);
		}
		break;

		// StimulusPresentation
	case ExperimentGovernor::StimulusPresentation:
		if (enabledStates.at(ExperimentGovernor::Congratulations) == true) {
			// Go to Instructions
			goToState(ExperimentGovernor::Congratulations);

		} else if (enabledStates.at(ExperimentGovernor::Instructions) == true) {
			// Go to Instructions
			goToState(ExperimentGovernor::Instructions);

		} else if (enabledStates.at(ExperimentGovernor::StimulusPresentation)) {
			// Nothing to do here

		} else {
			// Go to None
			goToState(ExperimentGovernor::None);

		}
		break;
		// Congratulations
	case ExperimentGovernor::Congratulations:
		if (enabledStates.at(ExperimentGovernor::Instructions) == true) {
			// Go to Instructions
			goToState(ExperimentGovernor::Instructions);

		} else {
			// Go to None
			goToState(ExperimentGovernor::None);

		}
		break;

		// None
	case ExperimentGovernor::None: 
		if (enabledStates.at(ExperimentGovernor::Instructions) == true) {
			// Go to Instructions
			goToState(ExperimentGovernor::Instructions);

		} else if (enabledStates.at(ExperimentGovernor::StimulusPresentation)) {
			// Go to StimulusPresentation
			goToState(ExperimentGovernor::StimulusPresentation);
		} else {
			// Nothing to do here
		}
		break;

	default:
		break;
	}
}

void ExperimentGovernor::update() {
	if ((_timeoutDelay > -1) &&  // _timeoutDelay is set
		((myGetElapsedTimeMillis() - _lastButtonDownTime) > _timeoutDelay*1000)) // We've exceeded _timeoutDelay
	{
		// Startover experiment
		_lastButtonDownTime = myGetElapsedTimeMillis();
		goToState(ExperimentGovernor::None);
	} else {
		switch (_currentState) {
		case ExperimentGovernor::None:
			nextState();
			break;
		case ExperimentGovernor::Instructions:
			// Refresh the instructions
			_instructionsPlayer->update();
			break;
		case ExperimentGovernor::BlankPage:
			if (_timedPagePlayer->update()) {
				nextState();
			}
			break;	
		case ExperimentGovernor::StimulusPresentation:
			// Refresh the Stimulus
			if (_stimulusPlayer->update() <= 0) {
				// Go to next state
				nextState();
			}		
			break;
		case ExperimentGovernor::Congratulations:
			if (_timedPagePlayer->update()) {
				nextState();
			}
			break;	
		default:
			break;
		}
	}
}

void ExperimentGovernor::buttonPressed() {
	_lastButtonDownTime = myGetElapsedTimeMillis();

	switch (_currentState) {
	case ExperimentGovernor::None:
		// Go to next state
		nextState();
		break;
	case ExperimentGovernor::Instructions:
		if (_instructionsPlayer->remaining() == 1) { 
			// Notify callbacks for logging
			unsigned long p = ++_participantNumber;
			unsigned long id = generateParticipantID(_participantNumber);
			ofNotifyEvent(newParticipant, id, this);

			_instructionsPlayer->setParticipantCode(id);
			_timedPagePlayer->setParticipantCode(id);
			_instructionsPlayer->buttonPressed();

		} else if (_instructionsPlayer->remaining() > 0) { 
			// If there are remaining instructions, go for it
			_instructionsPlayer->buttonPressed();
		} else {
			// Go to next state;
			nextState();
		}
		break;
	case ExperimentGovernor::StimulusPresentation:
		_stimulusPlayer->buttonPressed();
		break;
	default:
		// Nothing to do here
		break;
	}
}

unsigned long ExperimentGovernor::generateParticipantID(unsigned long participantNumber) 
{
	return (participantNumber ^ 313717);
}

unsigned long ExperimentGovernor::reverseParticipantID(unsigned long participantID) 
{
	return (participantID ^ 313717);
}

unsigned long ExperimentGovernor::getParticipantID() {
	return generateParticipantID(_participantNumber);
}

void ExperimentGovernor::setCongratulationsTime(float congratulationsTime) 
{
	_congratulationsTime = congratulationsTime;
}

TimedPagePlayer::TimedPagePlayer()
{
	_currentPage = TimedPagePlayer::BlankPage;
	_onDuration = 0.0;
	setParticipantCode(777777) ;
}

void TimedPagePlayer::setParticipantCode(unsigned long participantCode) 
{
	_participantCode = participantCode;
}

void TimedPagePlayer::start(float duration, pages page) 
{
	_currentPage = page;
	_onDuration = duration;
	_startTime = myGetElapsedTimeMillis();
}

bool TimedPagePlayer::update() 
{
	if ((myGetElapsedTimeMillis() - _startTime) < _onDuration*1000) {
		int i = _currentPage;
		ofNotifyEvent(drawPage, i, this);
		return false; // isDone == false
	} else {
		return true; // isDone == true
	}
}