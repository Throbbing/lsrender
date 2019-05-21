#include<thread/thread.h>

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