#include "ObjectInfo.h"
#include <iostream>

ObjectInfo::ObjectInfo() {}

ObjectInfo::ObjectInfo(ObjectState state)
: mState(state)
{}

ObjectInfoBuffer::ObjectInfoBuffer()
	: mTotalTimeToInterpolate(1000.0 / 20.0)
, mCurrentTime(0)
{}

ObjectInfoBuffer::ObjectInfoBuffer(object_info aStarting)
: ObjectInfoBuffer()
{
	mStarting = aStarting;
	mCurrent =  mStarting;
}

ObjectInfo ObjectInfoBuffer::GetNext(second aDeltaT) const
{
	if (mTargets.size() == 0)
	{
		return mStarting;
	}
	mCurrentTime += aDeltaT;

	if (mCurrentTime >= mTotalTimeToInterpolate)
	{
		mCurrentTime = 0.0;
		mStarting    = mTargets.front();
		mCurrent     = mStarting;
		mTargets.erase(mTargets.begin());

		//std::cout << mTargets.size() << std::endl;
		return mCurrent;
	}

	const auto time  = mCurrentTime / mTotalTimeToInterpolate;

	auto startState  = mStarting.GetState();
	auto targetState = mTargets.front().GetState();

	auto xVal = startState.mX * (1.0 - time) + targetState.mX * time;
	auto yVal = startState.mY * (1.0 - time) + targetState.mY * time;
	auto angleVal = startState.mAngle * (1.0 - time) + targetState.mAngle * time;
	auto xVelVal = startState.mVelX * (1.0 - time) + targetState.mVelX * time;
	auto yVelVal = startState.mVelY * (1.0 - time) + targetState.mVelY * time;
	auto rotVelVal = startState.mVelAngular * (1.0 - time) + targetState.mVelAngular * time;

	ObjectState pState;
	pState.mX = static_cast<float>(xVal);
	pState.mY = static_cast<float>(yVal);
	pState.mAngle = static_cast<float>(angleVal);
	pState.mVelX = static_cast<float>(xVelVal);
	pState.mVelY = static_cast<float>(yVelVal);
	pState.mVelAngular = static_cast<float>(rotVelVal);

	mCurrent.SetState(pState);

	return ObjectInfo(pState);
}

void ObjectInfoBuffer::AddTarget(object_info aTarget)
{
	if (mTargets.size() < 1) mTargets.push_back(aTarget);
}

void ObjectInfoBuffer::SetStartingInfo(const object_info& info)
{
	mCurrentTime = 0.0;
	mStarting    = info;
	mCurrent     = mStarting;
}