#pragma once
#include<config/config.h>
#include<config/common.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<thread/ThreadTask.h>
#include<thread/thread.h>
#include<queue>
#include<list>
/*
	队列形线程池
*/
namespace ls
{
	class QueuedThreadPool
	{
		friend class Thread;
		friend class ResourceManager;
	public:
		

		~QueuedThreadPool();


		/*
			添加新的任务
			若存在空闲线程，则直接调用空闲线程进行工作
			否则进入待完成任务队列
		*/
		void addTask(ThreadTaskPtr task);

		
	private:
		// 只能通过 资源管理器 才能创建线程池
		// -1 代表根据 实际的核心数创建线程
		QueuedThreadPool(s32 threadCount = -1);

		// 创建一个新的线程
		void addNewThread();


		


		

		std::mutex				mMutex;

		// 线程池中所有线程
		std::vector<ThreadPtr>	mAllThreads;

		// 空闲线程列表
		std::queue<ThreadPtr>	mFreeThreads;

		// 待完成任务队列
		std::queue<ThreadTaskPtr>	mToDoTasks;


					

	};

}