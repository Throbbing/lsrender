#pragma once
#include<config/config.h>


namespace ls
{
	class lsEmbree
	{
	public:
		
		
		static struct _hw{
			RTCDevice			rtcDevice = nullptr;
			RTCScene			rtcScene = nullptr;
		}hw;
		
		static void initEmbree();
		static void releaseEmbree();

	private:
		static bool				isInit;
	};




}


