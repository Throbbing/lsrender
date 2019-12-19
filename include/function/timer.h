#pragma once
#ifndef Timer_H_
#define Timer_H_
#include<iostream>
#include<windows.h>
using namespace std;

namespace ls
{
	enum TimeUnitType
	{
		ETimeUnit_Second = 0,
		ETimeUnit_MilliSecond = 1,
		ETimeUnit_Minute=2,
		ETimeUnit_Hour
	};
	class Timer
	{
	public:
		Timer();
		~Timer();


		void reset();
		void tick();
		void stop();
		float deltaTime()
		{
			return (float)mDeltaTime;
		}
		void start();
		float totalTime();


	private:
		__int64 mInitTime;//��ʼʱ��
		__int64 mStopTime;//��ͣʱ��
		__int64 mStartTime;//���¿�ʼ��ʱ��

		__int64 mPausedTime;//�ݶ�����ʱ��

		double mSecondsPerCount;
		double mDeltaTime;//ʱ����

		__int64 mCurrTime;//
		__int64 mLastTime;

		bool mIsStop;


	};
}


#endif // !Timer_H_
