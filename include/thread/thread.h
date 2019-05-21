#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<functional>
#include<memory>



namespace ls
{
	//前置声明

	class ThreadTask
	{
		friend ThreadWaker;
	public:
		ThreadTask(std::function<void(void)> task)
		{
			mTask = task;
			mOver = false;
		}
		bool	go()
		{
			return mFinish;
		}

		std::shared_ptr<ThreadWaker>	getWaker();

		void run();

	public:
		std::condition_variable		mCond;
		std::mutex					mCondMutex;

		bool						mOver;
		std::mutex					mOverMutex;

		bool						mFinish;
		static	u32					ii;
		u32							id;

		//任务
		std::function<void(void)>   mTask;
	};


	class ThreadWaker
	{
	public:
		ThreadWaker(std::condition_variable& cv, std::mutex& cm,
			bool& o, std::mutex& om, bool& f, ThreadTask* task) :
			mCond(cv), mCondMutex(cm),
			mOver(o), mOverMutex(om),
			mFinish(f),
			mTask(task)
		{

		}

		/*
		唤醒阻塞线程
		*/
		void wake()
		{
			mFinish = false;
			std::unique_lock <std::mutex> lg(mCondMutex);
			mCond.notify_all();
		}

		/*
		关闭阻塞线程
		*/
		void over()
		{
			std::lock_guard<std::mutex> lg(mOverMutex);
			mOver = true;
		}
	private:
		std::condition_variable&		mCond;
		std::mutex&						mCondMutex;
		bool&							mOver;
		std::mutex&						mOverMutex;

		bool&							mFinish;

		ThreadTask*						mTask;

	};

	//线程池，管理线程
	class ThreadPool
	{
	public:
		ThreadPool() {}
		~ThreadPool()
		{
			//把所有线程都设为关闭状态
			for (auto& p : mWakers)
				p->over();

			//唤醒任务关闭线程
			for (auto& p : mWakers)
				p->wake();

			//			std::cout << "Over" << std::endl;
			for (auto& p : mThreads)
				if (p.joinable())
					p.join();

			for (auto& p : mTasks)
				delete p;
		}

		std::shared_ptr<ThreadWaker> addTask(std::function<void(void)> task)
		{
			mTasks.push_back(new ThreadTask(task));
			mThreads.push_back(std::thread(&ThreadTask::run, mTasks.back()));
			mWakers.push_back(mTasks.back()->getWaker());

			return mWakers.back();
		}

		bool go(u32 index) { return mTasks[index]->go(); }

		u32	 tastCount() { return mTasks.size(); }

	public:
		std::vector<std::thread>		mThreads;
		std::vector<ThreadTask*>		mTasks;
		std::vector<std::shared_ptr<ThreadWaker>> mWakers;
	};
}