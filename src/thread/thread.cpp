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
		�����߳�
		*/
		std::unique_lock<std::mutex> lk(mCondMutex);
		mCond.wait(lk);
		lk.unlock();


		//����ǰ����Ƿ���߳��Ѿ���������
		{
			std::lock_guard<std::mutex> lg(mOverMutex);
			if (mOver)
			{
				std::cout << "���Ѻ�ر���" << std::endl;
				break;
			}
		}

		//����Task
		mTask();


		//Task���
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
	// �ر��߳�

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
	// �ر��߳�
	
	rhs.close();

	if (rhs.mThread.joinable())
		rhs.mThread.join();

	mThread = std::move(rhs.mThread);
	mThreadTask = rhs.mThreadTask;

	return *this;
}

void ls::Thread::executeTask(ThreadTaskPtr task)
{
	

	// ��鵱ǰ�Ƿ��������
	ls_AssertMsg(!mThreadTask, "Can't do more than one task at a time");


	// ������
	
	// ����
	std::lock_guard<std::mutex> lg(mCondMutex);

	mThreadTask = task;
	ls_MemoryBarrier;
	// �����߳�
	mThreadState = EThreadState_Ready;
	mCond.notify_all();
}

void ls::Thread::close()
{
	mClosed.store(false, std::memory_order_relaxed);

	// �����߳������߳��˳�
	std::unique_lock<std::mutex> lg(mCondMutex);
	mCond.notify_all();
}

void ls::Thread::run()
{
	// �ϲ�������ú���

	while (!mClosed.load(memory_order_relaxed))
	{
		
		// �̹߳��� �ȴ��µ�����
		std::unique_lock<std::mutex> lk(mCondMutex);
		mCond.wait(lk,
			[this]() {return mThreadState == EThreadState_Ready; });




		
		// �����ѣ����� mCondMutex ������������


		// �̻߳��Ѻ��ȼ���Ƿ��߳���Ҫ������
		// �������ڶ�ִ��һ�������µ�Ǳ������(������Դ�Ѿ�������)
		if (mClosed.load(memory_order_relaxed))
			break;

		// ����ִ�п�
		// �޸��߳�״̬ 
		mThreadState = EThreadState_Busy;
		
		{
			// �ж��Ƿ��������
			ls_AssertMsg(mThreadTask, "Invalid ThreadTask");

			// ������״̬�л�Ϊ ִ��
			mThreadTask->setThreadTaskState(EThreadTaskState_DOING);

			// ����������߳�ID
			mThreadTask->mThreadID = Thread::getThreadID();

			// ִ������
			mThreadTask->run();

			// ����ִ�����
			// ������״̬�л�Ϊ ���
			mThreadTask->setThreadTaskState(EThreadTaskState_DONE);
		}
		/*
			��������Ҫ����ִ��
			���߳�״̬��Ϊ ready �Ҳ���Ҫ����
		*/
		if (mThreadTask->goContinue())
		{
			// 
			mThreadTask->setThreadTaskState(EThreadTaskState_UNEXE);
			mThreadState = EThreadState_Ready;

			// ����Ҫ���̳߳ؽ��в���
			continue;
		}


		// �޸��߳�״̬
		mThreadState = EThreadState_Free;
		
		// �����߳��ÿ�
		mThreadTask->mThreadID = ThreadID();

		// �����ÿ�
		mThreadTask = nullptr;

		// �ع��̳߳�
		{
			// ���̳߳ؼ���
			std::lock_guard<std::mutex> lg(mThreadPool->mMutex);

			if (!mThreadPool->mToDoTasks.empty())
			{
				/*
					�����ڴ���������ʱ��ֱ�ӷ������ǰ�߳�
					���ٽ���ǰ�̼߳�����ж���
				*/
				mThreadTask = mThreadPool->mToDoTasks.front();
				mThreadPool->mToDoTasks.pop();

				
				mThreadState = EThreadState_Ready;
			}
			else
			{
				// ��������Ҫ��ɵ�����


				// ��ӽ������߳��б�
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




