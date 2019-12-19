#pragma once
#include<config/config.h>
#include<config/common.h>
#include<config/declaration.h>
#include<config/module.h>
#include<thread/thread.h>
#include<functional>

namespace ls
{
	/*
		实时渲染命令
	*/
	class RealtimeRenderCommand
	{
	public:
		// 判断当前命令是否在实时渲染线程中
		bool isRunOnRealtimeThread()
		{
			return ls::Thread::getThreadID() == lsEnvironment::realtimeRenderThreadID;
		}

		virtual const char* str() = 0;
		virtual void run(ThreadID dispatchID) = 0;
		virtual bool once() = 0;
	};
	
	/*
		模板实时渲染命令
		用以接受不同类型的可调用对象
	*/
	template<typename Desc,typename Func>
	class TRealtimeRenderCommand :public RealtimeRenderCommand
	{
	public:
		TRealtimeRenderCommand(Func&& inFunc, bool inIsOnce = false) :func(std::forward<Func>(inFunc)), isOnce(inIsOnce) {}



		virtual void run(ThreadID dispatchID) override
		{
			ls_AssertMsg(dispatchID == lsEnvironment::realtimeRenderThreadID, "Invalid dispatchID");
			func();
		}

		virtual const char* str() override
		{
			return Desc::str();
		}

		virtual bool once() override
		{
			return isOnce;
		}


	private:
		Func func;
		bool isOnce;
	};



	

}





