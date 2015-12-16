#ifndef TIMER_H
#define TIMER_H

#include <time.h>
#include <windows.h>

class Timer
{
public:
	typedef double milliseconds;

public:
	Timer();
	~Timer();

	//defines to start and stop timer
	#define TIMER_START QueryPerformanceCounter(&mStartTime);
	#define TIMER_STOP  QueryPerformanceCounter(&mEndTime);

	//returns true if enough time has passed
	bool   shouldUpdate();
	
	inline const milliseconds& getDeltaT() { return mDeltaT; };
	inline const milliseconds& getElapsedT() { return mElapsedT; };
private:
	//timer variables
	LARGE_INTEGER mStartTime;
	LARGE_INTEGER mFrequency;
	LARGE_INTEGER mEndTime;

	//how often RakNet should update and elapsed time
	milliseconds mRakNetFrameTime, mDeltaT, mElapsedT;

	//add time
	inline void addDelta(milliseconds delta) { mDeltaT += delta; };
	//calc elapsed time
	milliseconds calcDifferenceInMS();
};

#endif