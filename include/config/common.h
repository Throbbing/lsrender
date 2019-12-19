#pragma once
#include<config/config.h>
#include<config/lsPtr.h>
#include<string>
#include<d3d11.h>
#include<3rd/DirectX/WICTextureLoader.h>
#include<3rd/DirectX/DirectXTex.h>
#include<windows.h>
namespace ls
{
	void initRender();
	void releaseRender();


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



	class lsWnd
	{
	public:
		static HWND										wndHwnd;
		static HINSTANCE								wndHinstance;
		static u32										screenWidth;
		static u32										screenHeight;
		static f32										fps;


		static
			bool										lsWndInit(u32 w, u32 h, WNDPROC wndProc);

		static
			void										lsWndResize(u32 w, u32 h);

		static
			void										lsWndShutDown();


	};

	class lsRender
	{
	public:
		static
			struct _sampleInfo
		{
			s32 spp = 1;
			s32 iterations = 1;
			s32 directSamples = 1;
		}sampleInfo;

		static 
			struct _sceneRenderBlockInfo
		{
			s32 blockSizeX = 40;
			s32 blockSizeY = 40;
		}sceneRenderBlockInfo;

		static ScenePtr scene;

	};


	class lsEnvironment
	{
	public:
//		static TimerPtr timer;
		static QueuedThreadPoolPtr renderThreadPool; 
		static ThreadID	realtimeRenderThreadID;
		static RealtimeRendererPtr realtimeRenderer;

		static ThreadTaskPtr realtimeTask;
	};
	
	



}


