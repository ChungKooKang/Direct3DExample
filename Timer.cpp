#include "Timer.h"
#include <Windows.h>
namespace MyUtil
{
	Timer::Timer() : 
		mdSecondsPerCount{},
		mdDeltaTime{ -1.0 },
		mfScale { 1.0f },
		mllBaseTime{},
		mllPausedTime{},
		mllPrevTime{},
		mllCurrTime{},
		mllStopTime{},
		mbStopped{}
	{
		long long countsPerSec; // 1초에 몇번 일하니?
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);

		mdSecondsPerCount = 1.0 / static_cast<double>(countsPerSec); // CPU가 한번 일할 때 걸리는 시간
	}
	void Timer::Start()
	{
		long long currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);		// 컴퓨터가 전원이 켜진 이후에 몇 번 일함?

		mllBaseTime = currTime;

		mllPrevTime = currTime;
		mllStopTime = 0;

		mbStopped = false;
		
	}
	void Timer::Stop()
	{
		if (!mbStopped)
		{
			long long currTime;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

			mllStopTime = currTime;
			mbStopped = true;
		}
	}
	void Timer::Resume()
	{
		long long currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);

		if (mbStopped)
		{
			mllPausedTime += (currTime - mllStopTime);

			mllPrevTime = currTime;
			mllStopTime = 0;
			mbStopped = false;
		}
	}
	void Timer::Update()
	{
		if (mbStopped)
		{
			mdDeltaTime = 0.0;
			return;
		}

		long long currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);
		mllCurrTime = currTime;

		mdDeltaTime = (mllCurrTime - mllPrevTime) * mdSecondsPerCount;

		mllPrevTime - currTime;

		if (mdDeltaTime < 0.0)
		{
			mdDeltaTime = 0.0;
		}
	}
	float Timer::Totaltime() const
	{
		return 0.0f;
	}
	float Timer::DeltaTime() const
	{
		return 0.0f;
	}
	void Timer::SetScale(float scale)
	{
	}
}