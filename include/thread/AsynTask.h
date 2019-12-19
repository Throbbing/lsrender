#pragma once
#include<config/config.h>
#include<function/log.h>
#include<thread/ThreadTask.h>

/*
	异步线程任务类，仅负责维护绑定的具体任务实例
	不负责实际线程维护
*/

/*
	只执行一次的任务
*/

namespace ls
{
	template<typename TTask>
	class AsynOnceTask :public ThreadTask
	{
	public:
		/*
			构造函数，接受实际任务所需要的参数
		*/
		template<typename... TaskArgs>
		AsynOnceTask(TaskArgs&&... args) :mTask(std::forward<TaskArgs>(args)...) {}


		virtual void run() override
		{
			// 调用 可调用对象
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
		// 任意的可调用对象 (无参数)
		TTask		mTask;



	};


	/*
		循环执行类
	*/
#define AsynLoopInfinite -1

	template<typename TTask>
	class AsynLoopTask :public ThreadTask
	{
	public:
		/*
			构造函数，接受循环次数和任务实际所需的参数
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

			// 当且仅当 非无限循环才计算循环次数
			if (mLoopCount > 0)
				++mLoopIndex;
		}

		virtual bool goContinue() final
		{
			// 无限循环
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