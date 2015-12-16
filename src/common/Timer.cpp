#include "Timer.h"
#include <iostream>

Timer::Timer()
{
	mRakNetFrameTime = 1000.0 / 30.0; //30 fps
	mDeltaT = 0.0; //no time has elapsed yet

	QueryPerformanceFrequency(&mFrequency); //get frequency
	TIMER_START //start timer
}

Timer::~Timer() {}

Timer::milliseconds Timer::calcDifferenceInMS()
{
	milliseconds difference = (milliseconds)(mEndTime.QuadPart - mStartTime.QuadPart) / (milliseconds)(mFrequency.QuadPart);
	return difference * 1000;
}

bool Timer::shouldUpdate()
{
	bool shouldUpdate = false;


	TIMER_STOP //end timer

	addDelta(calcDifferenceInMS()); //calculate and add elapsed time
	mElapsedT = calcDifferenceInMS();

	if (mDeltaT >= mRakNetFrameTime) //if it is time to update raknet
	{
		mDeltaT = mDeltaT - mRakNetFrameTime; //don't forget excess time

		shouldUpdate = true; //return true
	}

	TIMER_START //reset start time

	return shouldUpdate; //else return false
}