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