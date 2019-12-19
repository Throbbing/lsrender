#include<function/log.h>
#include<thread/thread.h>
#include<thread/ThreadTask.h>
#include<thread/QueuedThreadPool.h>
#if 0
u32 ls::ThreadTask::ii = 0;

std::shared_ptr<ls::ThreadWaker>
ls::ThreadTask::getWaker()
{
	return std::shared_ptr<ls::ThreadWaker>(new
		ThreadWaker(mCond, mCondMutex, mOver, mOverMutex,
			mFinish, this));
}

void ls::ThreadTask::run()
{
	static u32 count = 1;
	while (true)
	{
		{
			std::lock_guard<std::mutex> lg(mOverMutex);
			if (mOver)
			{
				std::cout << "Break" << std::endl;
				break;
			}
		}


		/*
		挂起线程
		*/
		std::unique_lock<std::mutex> lk(mCondMutex);
		mCond.wait(lk);
		lk.unlock();


		//运行前检查是否该线程已经被结束了
		{
			std::lock_guard<std::mutex> lg(mOverMutex);
			if (mOver)
			{
				std::cout << "唤醒后关闭了" << std::endl;
				break;
			}
		}

		//运行Task
		mTask();


		//Task完成
		mFinish = true;
		++count;


	}
}
#endif

ls::Thread::Thread(QueuedThreadPoolPtr poolPtr):
	Module("Thread"),
	mThreadTask(nullptr),
	mClosed(false),
	mThread(&ls::Thread::run,this),
	mThreadPool(poolPtr)
{
	
}


ls::Thread::Thread(Thread && rhs):Module("Thread")
{
	// 关闭线程

	rhs.close();

	if (rhs.mThread.joinable())
		rhs.mThread.join();
	
	if (rhs.mClosed.load(memory_order_relaxed))
		mClosed.store(true, memory_order_relaxed);
	else
		mClosed.store(false, memory_order_relaxed);
	mThread = std::move(rhs.mThread);
	mThreadTask = rhs.mThreadTask;
}

ls::Thread& ls::Thread::operator=(Thread && rhs)
{
	// 关闭线程
	
	rhs.close();

	if (rhs.mThread.joinable())
		rhs.mThread.join();

	mThread = std::move(rhs.mThread);
	mThreadTask = rhs.mThreadTask;

	return *this;
}

void ls::Thread::executeTask(ThreadTaskPtr task)
{
	

	// 检查当前是否存在任务
	ls_AssertMsg(!mThreadTask, "Can't do more than one task at a time");


	// 绑定任务
	
	// 加锁
	std::lock_guard<std::mutex> lg(mCondMutex);

	mThreadTask = task;
	ls_MemoryBarrier;
	// 唤醒线程
	mThreadState = EThreadState_Ready;
	mCond.notify_all();
}

void ls::Thread::close()
{
	mClosed.store(false, std::memory_order_relaxed);

	// 唤醒线程以让线程退出
	std::unique_lock<std::mutex> lg(mCondMutex);
	mCond.notify_all();
}

void ls::Thread::run()
{
	// 上层任务调用函数

	while (!mClosed.load(memory_order_relaxed))
	{
		
		// 线程挂起 等待新的任务
		std::unique_lock<std::mutex> lk(mCondMutex);
		mCond.wait(lk,
			[this]() {return mThreadState == EThreadState_Ready; });




		
		// 任务唤醒，处于 mCondMutex 保护作用域中


		// 线程唤醒后，先检查是否线程需要被销毁
		// 避免由于多执行一次任务导致的潜在问题(所需资源已经被销毁)
		if (mClosed.load(memory_order_relaxed))
			break;

		// 任务执行块
		// 修改线程状态 
		mThreadState = EThreadState_Busy;
		
		{
			// 判断是否存在任务
			ls_AssertMsg(mThreadTask, "Invalid ThreadTask");

			// 将任务状态切换为 执行
			mThreadTask->setThreadTaskState(EThreadTaskState_DOING);

			// 给任务分配线程ID
			mThreadTask->mThreadID = Thread::getThreadID();

			// 执行任务
			mThreadTask->run();

			// 任务执行完成
			// 将任务状态切换为 完毕
			mThreadTask->setThreadTaskState(EThreadTaskState_DONE);
		}
		/*
			若任务需要继续执行
			则将线程状态设为 ready 且不需要挂起
		*/
		if (mThreadTask->goContinue())
		{
			// 
			mThreadTask->setThreadTaskState(EThreadTaskState_UNEXE);
			mThreadState = EThreadState_Ready;

			// 不需要对线程池进行操作
			continue;
		}


		// 修改线程状态
		mThreadState = EThreadState_Free;
		
		// 任务线程置空
		mThreadTask->mThreadID = ThreadID();

		// 任务置空
		mThreadTask = nullptr;

		// 回归线程池
		{
			// 对线程池加锁
			std::lock_guard<std::mutex> lg(mThreadPool->mMutex);

			if (!mThreadPool->mToDoTasks.empty())
			{
				/*
					当存在待分配任务时，直接分配给当前线程
					不再将当前线程加入空闲队列
				*/
				mThreadTask = mThreadPool->mToDoTasks.front();
				mThreadPool->mToDoTasks.pop();

				
				mThreadState = EThreadState_Ready;
			}
			else
			{
				// 不存在需要完成的任务


				// 添加进空闲线程列表
				mThreadPool->mFreeThreads.push(this);
			}
		}
	}
}

void ls::Thread::commit()
{
}

std::string ls::Thread::strOut() const
{
	return std::string();
}




