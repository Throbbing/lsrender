#pragma once
#include<config/config.h>
#include<config/common.h>
#include<config/declaration.h>
#include<config/module.h>

namespace ls
{
	// ����״̬
	enum ThreadTaskStateType
	{
		// ����δִ��
		EThreadTaskState_UNEXE = 0,
		// ����ִ����
		EThreadTaskState_DOING = 1,
		// �������
		EThreadTaskState_DONE = 2,
		// �����쳣
		EThreddTaskState_ERROR = 3
	};
	// �߳�������ӿ�
	// һ������ֻ�ܰ���һ���߳���

	class ThreadTask:public Module
	{
		friend class Thread;
	public:
		ThreadTask():Module("ThreadTask")
		{
			setThreadTaskState(EThreadTaskState_UNEXE);
		}
		virtual ~ThreadTask() {}

		/*
			ִ�и� Task ���󶨵�ʵ������
		*/
		virtual void run() = 0;

		/*
			������ɺ��Ƿ����
		*/
		virtual bool goContinue() = 0;
		
		/*
			����������
		*/
		virtual void abandon() = 0;

		auto getThreadTaskState() const
		{
			return mThreadTaskState;
		}

		// �������и�������߳� ID
		auto getRunningThreadID() const
		{
			return mThreadID;
		}

		virtual void commit() override {

		}

		virtual std::string strOut() const override
		{
			return "";
		}

	private:
		// ����״ֻ̬���� �̸߳ı�
		void setThreadTaskState(ThreadTaskStateType taskState)
		{
			mThreadTaskState = taskState;
		}

		ThreadTaskStateType mThreadTaskState;
		ThreadID			mThreadID; 

	};
}