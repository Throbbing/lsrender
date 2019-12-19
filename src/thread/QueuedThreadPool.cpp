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

	// �̳߳ؼ���
	std::lock_guard<std::mutex> lg(mMutex);

	// �жϿ����̳߳�
	if (!mFreeThreads.empty())
	{
		// ��ȡ�����߳�
		auto freeThread = mFreeThreads.front();
		mFreeThreads.pop();

		// ��������
		freeThread->executeTask(task);
		return;
	}

	// �����ڿ����̣߳���������������������
	mToDoTasks.push(task);

}

void ls::QueuedThreadPool::addNewThread()
{
	// �¹������̶߳�Ϊ Free ״̬
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