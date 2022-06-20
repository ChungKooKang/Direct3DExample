#pragma once


//��ɵ�
// ����
// ����
// �簳

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

		void Update(); // ���� �߿�! �׻� ������ �ϵ� ������Ʈ ����� ��!

		float Totaltime() const;
		float DeltaTime() const;

		void SetScale(float scale);
	private :
		double mdSecondsPerCount;	// count�� �ѹ� ���� �� �� �ʰ� �ɸ������� ����.
		double mdDeltaTime;			// ������ �۾��� �̹� �۾��� �ð� ����
		float mfScale;

		long long mllBaseTime;
		long long mllPausedTime;
		long long mllStopTime;
		long long mllPrevTime;
		long long mllCurrTime;

		bool mbStopped;
	};
}

