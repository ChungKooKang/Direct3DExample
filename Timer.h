#pragma once


//기능들
// 시작
// 멈춤
// 재개

namespace MyUtil
{
	class Timer
	{
	public :
		Timer();

	public :
		void Start();
		void Stop();
		void Resume();

		void Update(); // 가장 중요! 항상 무엇을 하든 업데이트 해줘야 함!

		float Totaltime() const;
		float DeltaTime() const;

		void SetScale(float scale);
	private :
		double mdSecondsPerCount;	// count는 한번 일할 때 몇 초가 걸리는지를 말함.
		double mdDeltaTime;			// 이전에 작업과 이번 작업의 시간 차이
		float mfScale;

		long long mllBaseTime;
		long long mllPausedTime;
		long long mllStopTime;
		long long mllPrevTime;
		long long mllCurrTime;

		bool mbStopped;
	};
}

