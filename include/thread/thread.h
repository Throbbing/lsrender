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
	�̹߳����
	�����ܵ� ThreadEvent ��Ӧ���̹߳���
	
	ֻ���� ls::Thread �е���
	һ����������ֻ�ܵ���һ��
*/

#define ls_WaitThread(threadEvent) std::unique_lock<std::mutex> ls_WaitThread_Unique_Lock(threadEvent->mCondMutex);\
	threadEvent->mCond.wait(ls_WaitThread_Unique_Lock);
	

/*
	�ֶ����� ls_WaitThread ��ӵ�е� mutex
*/
#define ls_UnlockWaitThreadMutex(threadEvent) ls_WaitThread_Unique_Lock.unlock();
	

/*
	�̹߳����
	�����ܵ� ThreadEvent ��Ӧ���̣߳��Ǿ���̬������ 
*/
	
#define ls_WaitThreadReady(threadEvent) std::unique_lock<std::mutex> ls_WaitThreadReady_Unique_Lock(threadEvent->mCondMutex); \
threadEvent->mCond.wait(ls_WaitThreadReady_Unique_Lock, \
	[this]() { return threadEvent->mThreadState == EThreadState_Ready; });


/*
	�ֶ����� ls_WaitThreadReady ��ӵ�е� mutex
*/
#define ls_UnlockWaitThreadReady(threadEvent) ls_WaitThreadReady_Unique_Lock.unlock();
#endif
	


namespace ls
{

#if 0
	// ǰ������
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

		//����
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
		���������߳�
		*/
		void wake()
		{
			mFinish = false;
			std::unique_lock <std::mutex> lg(mCondMutex);
			mCond.notify_all();
		}

		/*
		�ر������߳�
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

	//�̳߳أ������߳�
	class ThreadPool
	{
	public:
		ThreadPool() {}
		~ThreadPool()
		{
			//�������̶߳���Ϊ�ر�״̬
			for (auto& p : mWakers)
				p->over();

			//��������ر��߳�
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
		�߳��࣬����ά��ʵ�ʵĵײ��߳� 
		�ײ��߳�������׼��Ķ��߳�֧�֣���������Ҫ֧�� C++ 11 ���ϰ汾��
		
	*/

	enum ThreadStateType
	{
		// �߳̿���
		EThreadState_Free = 0,
		// �߳�æµ
		EThreadState_Busy  = 1,
		// �߳�׼������ 
		EThreadState_Ready = 2
	};



	class Thread:public Module
	{
		
	public:
		/*
			Ĭ�Ϲ��캯��
		*/
		Thread(QueuedThreadPoolPtr poolPtr);

		/*
			��������
		*/
		~Thread()
		{
			if (mThread.joinable())
				mThread.join();
		}

		/*
			��ȡ��ǰ�����߳� ID	
		*/
		static ThreadID getThreadID()
		{	
			return std::this_thread::get_id();
		}



		/*
			���� std::thread ֻ�����ƶ����죬���Խ�ֹ�߳���Ŀ�������͸�ֵ
		*/
		Thread(const Thread&) = delete;
		Thread& operator=(const Thread&) = delete;

		/*
			�ƶ�����͸�ֵ����
		*/
		Thread(Thread&& rhs);
		Thread& operator=(Thread&& rhs);


		// �����̰߳�һ������
		void executeTask(ThreadTaskPtr task);
		
		
		/*
			�ر��߳�
		*/
		void close();


	private:

		/*
			�ϲ�������ú���
			����� std::thread ���а�
		*/
		void run();

		virtual void commit() override;
		virtual std::string strOut() const  override;


		// �߳�����
		ls::ThreadTaskPtr		mThreadTask = nullptr;


		// �߳�״̬
		ls::ThreadStateType		mThreadState;

		// ��������
		std::condition_variable	mCond;

		// �����������õ� ������
		std::mutex				mCondMutex;

		// ����߳��Ƿ���Ҫ�رյ�ԭ�ӱ���
		std::atomic_bool		mClosed;

		// �������߳�ʵ��
		std::thread				mThread;



		// �������̵߳��̳߳�
		ls::QueuedThreadPoolPtr	mThreadPool;

	};
}