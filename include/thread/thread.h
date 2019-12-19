#pragma once
#include<config/config.h>
#include<config/common.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
#include<function/timer.h>
#include<thread>
#include<mutex>
#include<condition_variable>
#include<atomic>
#include<functional>
#include<memory>
#include<mmintrin.h>


#define ls_MemoryBarrier { _mm_sfence();}

#if 0
/*
	线程挂起宏
	将接受的 ThreadEvent 对应的线程挂起
	
	只能在 ls::Thread 中调用
	一个作用域内只能调用一次
*/

#define ls_WaitThread(threadEvent) std::unique_lock<std::mutex> ls_WaitThread_Unique_Lock(threadEvent->mCondMutex);\
	threadEvent->mCond.wait(ls_WaitThread_Unique_Lock);
	

/*
	手动解锁 ls_WaitThread 所拥有的 mutex
*/
#define ls_UnlockWaitThreadMutex(threadEvent) ls_WaitThread_Unique_Lock.unlock();
	

/*
	线程挂起宏
	将接受的 ThreadEvent 对应的线程（非就绪态）挂起 
*/
	
#define ls_WaitThreadReady(threadEvent) std::unique_lock<std::mutex> ls_WaitThreadReady_Unique_Lock(threadEvent->mCondMutex); \
threadEvent->mCond.wait(ls_WaitThreadReady_Unique_Lock, \
	[this]() { return threadEvent->mThreadState == EThreadState_Ready; });


/*
	手动解锁 ls_WaitThreadReady 所拥有的 mutex
*/
#define ls_UnlockWaitThreadReady(threadEvent) ls_WaitThreadReady_Unique_Lock.unlock();
#endif
	


namespace ls
{

#if 0
	// 前置声明
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
#endif
	/*
		线程类，负责维护实际的底层线程 
		底层线程依赖标准库的多线程支持（编译器需要支持 C++ 11 以上版本）
		
	*/

	enum ThreadStateType
	{
		// 线程空闲
		EThreadState_Free = 0,
		// 线程忙碌
		EThreadState_Busy  = 1,
		// 线程准备就绪 
		EThreadState_Ready = 2
	};



	class Thread:public Module
	{
		
	public:
		/*
			默认构造函数
		*/
		Thread(QueuedThreadPoolPtr poolPtr);

		/*
			析构函数
		*/
		~Thread()
		{
			if (mThread.joinable())
				mThread.join();
		}

		/*
			获取当前运行线程 ID	
		*/
		static ThreadID getThreadID()
		{	
			return std::this_thread::get_id();
		}



		/*
			由于 std::thread 只允许移动构造，所以禁止线程类的拷贝构造和赋值
		*/
		Thread(const Thread&) = delete;
		Thread& operator=(const Thread&) = delete;

		/*
			移动构造和赋值函数
		*/
		Thread(Thread&& rhs);
		Thread& operator=(Thread&& rhs);


		// 给该线程绑定一个任务
		void executeTask(ThreadTaskPtr task);
		
		
		/*
			关闭线程
		*/
		void close();


	private:

		/*
			上层任务调用函数
			负责和 std::thread 进行绑定
		*/
		void run();

		virtual void commit() override;
		virtual std::string strOut() const  override;


		// 线程任务
		ls::ThreadTaskPtr		mThreadTask = nullptr;


		// 线程状态
		ls::ThreadStateType		mThreadState;

		// 条件变量
		std::condition_variable	mCond;

		// 条件变量所用的 互斥量
		std::mutex				mCondMutex;

		// 标记线程是否需要关闭的原子变量
		std::atomic_bool		mClosed;

		// 真正的线程实例
		std::thread				mThread;



		// 创建该线程的线程池
		ls::QueuedThreadPoolPtr	mThreadPool;

	};
}