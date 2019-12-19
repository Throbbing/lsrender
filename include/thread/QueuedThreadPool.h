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
	�������̳߳�
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
			����µ�����
			�����ڿ����̣߳���ֱ�ӵ��ÿ����߳̽��й���
			������������������
		*/
		void addTask(ThreadTaskPtr task);

		
	private:
		// ֻ��ͨ�� ��Դ������ ���ܴ����̳߳�
		// -1 ������� ʵ�ʵĺ����������߳�
		QueuedThreadPool(s32 threadCount = -1);

		// ����һ���µ��߳�
		void addNewThread();


		


		

		std::mutex				mMutex;

		// �̳߳��������߳�
		std::vector<ThreadPtr>	mAllThreads;

		// �����߳��б�
		std::queue<ThreadPtr>	mFreeThreads;

		// ������������
		std::queue<ThreadTaskPtr>	mToDoTasks;


					

	};

}