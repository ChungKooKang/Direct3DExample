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
		long long countsPerSec; // 1�ʿ� ��� ���ϴ�?
		QueryPerformanceFrequency((LARGE_INTEGER*)&countsPerSec);

		mdSecondsPerCount = 1.0 / static_cast<double>(countsPerSec); // CPU�� �ѹ� ���� �� �ɸ��� �ð�
	}
	void Timer::Start()
	{
		long long currTime;
		QueryPerformanceCounter((LARGE_INTEGER*)&currTime);		// ��ǻ�Ͱ� ������ ���� ���Ŀ� �� �� ����?

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

		mllPrevTime = currTime;

		if (mdDeltaTime < 0.0)
		{
			mdDeltaTime = 0.0;
		}
	}
	float Timer::Totaltime() const
	{
		if ( mbStopped )
		{
			return static_cast<float>(((mllStopTime - mllPausedTime) - mllBaseTime) * mdSecondsPerCount); // ���� �������� ���� ���� �ð����� �� ��� �Ѵ�.
		} else
		{
			return static_cast<float>(((mllCurrTime - mllPausedTime) - mllBaseTime) * mdSecondsPerCount);
		}
	}
	float Timer::DeltaTime() const
	{
		return static_cast<float>(mdDeltaTime * mfScale);	// mfScale�� �ð� ����� �ǹ��Ѵ�.
	}
	void Timer::SetScale(float scale)
	{
		mfScale = scale;
	}
}