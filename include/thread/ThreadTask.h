#pragma once
#include<config/config.h>
#include<config/common.h>
#include<config/declaration.h>
#include<config/module.h>

namespace ls
{
	// 任务状态
	enum ThreadTaskStateType
	{
		// 任务未执行
		EThreadTaskState_UNEXE = 0,
		// 任务执行中
		EThreadTaskState_DOING = 1,
		// 任务完成
		EThreadTaskState_DONE = 2,
		// 任务异常
		EThreddTaskState_ERROR = 3
	};
	// 线程任务类接口
	// 一个任务只能绑定在一个线程上

	class ThreadTask:public Module
	{
		friend class Thread;
	public:
		ThreadTask():Module("ThreadTask")
		{
			setThreadTaskState(EThreadTaskState_UNEXE);
		}
		virtual ~ThreadTask() {}

		/*
			执行该 Task 所绑定的实际任务
		*/
		virtual void run() = 0;

		/*
			任务完成后是否继续
		*/
		virtual bool goContinue() = 0;
		
		/*
			放弃该任务
		*/
		virtual void abandon() = 0;

		auto getThreadTaskState() const
		{
			return mThreadTaskState;
		}

		// 返回运行该任务的线程 ID
		auto getRunningThreadID() const
		{
			return mThreadID;
		}

		virtual void commit() override {

		}

		virtual std::string strOut() const override
		{
			return "";
		}

	private:
		// 任务状态只能由 线程改变
		void setThreadTaskState(ThreadTaskStateType taskState)
		{
			mThreadTaskState = taskState;
		}

		ThreadTaskStateType mThreadTaskState;
		ThreadID			mThreadID; 

	};
}