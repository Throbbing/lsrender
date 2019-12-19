#include <function/log.h>
#include <thread/QueuedThreadPool.h>
#include <resource/resourceManager.h>


ls::QueuedThreadPool::~QueuedThreadPool()
{
	// 
	for (auto& t : mAllThreads)
	{
		t->close();
		delete t;
		t = nullptr;
	}

}

void ls::QueuedThreadPool::addTask(ThreadTaskPtr task)
{
	ls_Assert(task);

	// 线程池加锁
	std::lock_guard<std::mutex> lg(mMutex);

	// 判断空闲线程池
	if (!mFreeThreads.empty())
	{
		// 获取空闲线程
		auto freeThread = mFreeThreads.front();
		mFreeThreads.pop();

		// 分配任务
		freeThread->executeTask(task);
		return;
	}

	// 不存在空闲线程，将任务加入待完成任务队列
	mToDoTasks.push(task);

}

void ls::QueuedThreadPool::addNewThread()
{
	// 新构建的线程都为 Free 状态
	ThreadPtr newThreadPtr = new ls::Thread(this);
	ls_Assert(newThreadPtr);

	mAllThreads.push_back(newThreadPtr);
	mFreeThreads.push(newThreadPtr);
}


ls::QueuedThreadPool::QueuedThreadPool(
	s32 threadCount)
{
	if (threadCount < 0)
		threadCount = std::thread::hardware_concurrency() - 1;

	for (s32 i = 0; i < threadCount; ++i)
		addNewThread();
}