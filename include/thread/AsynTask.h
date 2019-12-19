#pragma once
#include<config/config.h>
#include<function/log.h>
#include<thread/ThreadTask.h>

/*
	�첽�߳������࣬������ά���󶨵ľ�������ʵ��
	������ʵ���߳�ά��
*/

/*
	ִֻ��һ�ε�����
*/

namespace ls
{
	template<typename TTask>
	class AsynOnceTask :public ThreadTask
	{
	public:
		/*
			���캯��������ʵ����������Ҫ�Ĳ���
		*/
		template<typename... TaskArgs>
		AsynOnceTask(TaskArgs&&... args) :mTask(std::forward<TaskArgs>(args)...) {}


		virtual void run() override
		{
			// ���� �ɵ��ö���
			mTask();
		}

		bool goContinue() final
		{
			return false;
		}

		virtual void abandon() override
		{
			Unimplement;
		}



	protected:
		// ����Ŀɵ��ö��� (�޲���)
		TTask		mTask;



	};


	/*
		ѭ��ִ����
	*/
#define AsynLoopInfinite -1

	template<typename TTask>
	class AsynLoopTask :public ThreadTask
	{
	public:
		/*
			���캯��������ѭ������������ʵ������Ĳ���
		*/
		template<typename... TaskArgs>
		AsynLoopTask(s32 loopCount, TaskArgs&&... args) :
			mTask(std::forward<TaskArgs>(args)...), mLoopIndex(0), mLoopCount(loopCount)
		{
			ls_AssertMsg(mLoopCount != 0, "Invalid LoopCount!");
		}

		virtual void run() override
		{
			mTask();

			// ���ҽ��� ������ѭ���ż���ѭ������
			if (mLoopCount > 0)
				++mLoopIndex;
		}

		virtual bool goContinue() final
		{
			// ����ѭ��
			if (mLoopCount < 0)
				return true;

			if (mLoopIndex >= mLoopCount)
				return false;

			return true;
		}

		virtual void abandon() override
		{
			Unimplement;
		}

	protected:
		TTask			mTask;

		s32				mLoopIndex;
		s32				mLoopCount;
	};
}