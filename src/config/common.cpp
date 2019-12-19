#include "..\..\include\config\common.h"

#include<config/common.h>
#include<3rd/imgui.h>
#include<function/timer.h>
#include<function/log.h>
#include<scene/scene.h>
#include<thread/QueuedThreadPool.h>
#include<d3d11.h>
#include<DirectXMath.h>
#include<D3dcsx.h>
#include<d3dCompiler.h>
#include<3rd/DirectX/WICTextureLoader.h>
#include<3rd/DirectX/dxerr.h>
#include<3rd/DirectX/DirectXTex.h>

#include<windows.h>
#include<tchar.h>
#include<resource/resourceManager.h>
#include<realtime/realtime.h>
#include<realtime/D3D11Realtime.h>
#include<thread/ThreadTask.h>
#include<thread/AsynTask.h>
#include<thread/QueuedThreadPool.h>
bool ls::lsEmbree::isInit = false;
ls::lsEmbree::_hw ls::lsEmbree::hw;
void ls::lsEmbree::initEmbree()
{
	if (isInit)
		return;

	hw.rtcDevice = rtcNewDevice(nullptr);
	hw.rtcScene = rtcNewScene(hw.rtcDevice);

	isInit = true;
}


void ls::lsEmbree::releaseEmbree()
{
	if (hw.rtcScene)
		rtcReleaseScene(hw.rtcScene);

	if (hw.rtcDevice)
		rtcReleaseDevice(hw.rtcDevice);
}


extern LRESULT CALLBACK wndGUIProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND				ls::lsWnd::wndHwnd = nullptr;
HINSTANCE			ls::lsWnd::wndHinstance = nullptr;
u32					ls::lsWnd::screenWidth = 0;
u32					ls::lsWnd::screenHeight = 0;
f32					ls::lsWnd::fps = 0.f;



bool ls::lsWnd ::lsWndInit(u32 w, u32 h, WNDPROC wndProc)
{
#if 1
	screenWidth = w;
	screenHeight = h;

	// 初始化 ImGui
	ImGui::CreateContext();
	
	
	wndHinstance = GetModuleHandle(NULL);

	WNDCLASSEX wc = { sizeof(WNDCLASSEX), CS_CLASSDC, wndProc, 0L, 0L, wndHinstance, NULL, LoadCursor(NULL, IDC_ARROW), NULL, NULL, _T("lsRender"), NULL };
	if (!RegisterClassEx(&wc))
	{
		std::cout << "Error: RegisterClassEx(&wc)" << std::endl;
		return false;
	}
	RECT R = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;

	wndHwnd = CreateWindowA("lsRender", "lsRender",
		WS_OVERLAPPEDWINDOW, 0, 0, width, height, 0, 0, wndHinstance, 0);

	if (!wndHwnd)
	{
		std::cout << "Error: CreateWindow()" << std::endl;
		return false;
	}


	ShowWindow(wndHwnd, SW_SHOW);
	UpdateWindow(wndHwnd);

#endif
	return true;
}

void ls::lsWnd::lsWndResize(u32 w, u32 h)
{
	screenWidth = w;
	screenHeight = h;

	ImGui::DestroyContext();
	ImGui::CreateContext();

	RECT R = { 0, 0, screenWidth, screenHeight };
	AdjustWindowRect(&R, WS_OVERLAPPEDWINDOW, false);
	int width = R.right - R.left;
	int height = R.bottom - R.top;
	GetWindowRect(lsWnd::wndHwnd, &R);
	MoveWindow(lsWnd::wndHwnd, R.left, R.top, width, height, true);
	
}

void ls::lsWnd::lsWndShutDown()
{



}


ls::lsRender::_sampleInfo  ls::lsRender::sampleInfo;
ls::lsRender::_sceneRenderBlockInfo ls::lsRender::sceneRenderBlockInfo;
ls::ScenePtr ls::lsRender::scene = nullptr;
ls::QueuedThreadPoolPtr ls::lsEnvironment::renderThreadPool = nullptr;
ls::RealtimeRendererPtr ls::lsEnvironment::realtimeRenderer = nullptr;
ls::ThreadTaskPtr		ls::lsEnvironment::realtimeTask = nullptr;
ThreadID				ls::lsEnvironment::realtimeRenderThreadID;
//ls::TimerPtr ls::lsEnvironment::timer = new Timer();

void ls::initRender()
{
	ls::lsEmbree::initEmbree();
	ls::lsEnvironment::renderThreadPool = ls::ResourceManager::createThreadPool(-1);

	// 创建实时渲染线程
	ls::lsEnvironment::realtimeTask = new ls::AsynOnceTask<D3DRealtimeThreadTask>();

	// 添加渲染线程
	ls::lsEnvironment::renderThreadPool->addTask(ls::lsEnvironment::realtimeTask);
	
	while (!(ls::lsEnvironment::realtimeTask->getThreadTaskState() == EThreadTaskState_DOING))
	{
		Sleep(500);
	}
	ls::lsEnvironment::realtimeRenderThreadID = ls::lsEnvironment::realtimeTask->getRunningThreadID();


}

void ls::releaseRender()
{
	ls::lsEmbree::releaseEmbree();

	
}
