#include<iostream>
#include<math/math.h>
#include<stdlib.h>
#include<embree3/rtcore.h>
#include<function/file.h>
#include<film/film.h>
#include<thread/QueuedThreadPool.h>
#include<thread/AsynTask.h>
#include<stdio.h>
#include<stdarg.h>
#include<cwchar>
#include<resource\xmlHelper.h>
#include<scene\scene.h>
#include<resource\resourceManager.h>
#include<config\common.h>
#include<function/timer.h>
#include<realtime/D3D11Realtime.h>

#define PrePath "SceneFile//cbox//"
#define FileName "cbox.xml"
#define OutName "cbox-bdpt.png"


extern LRESULT CALLBACK wndGUIProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//int main()
//{
//	ls::initRender();
//	
////	ls::ResourceManager::loadTextureFromFileW("cbox", L"skylight-morn.exr");
//
//	auto package = ls::XMLParser::loadXMLFromMTSFile(PrePath, 
//		FileName);
//
////	ls::XMLParser::printXMLPackage(package);
//	ls::ResourceManager::setPath(PrePath);
//	auto scene = ls::ResourceManager::createSceneObj();
//	scene->setSceneFromXML(PrePath, package);
//	scene->render();
//	ls::ResourceManager::write2File(scene->getMainFilm()->convert2Texture(),
//		OutName);
//	std::cout << "FINISHED!" << std::endl;
//	
//	
//	ls::releaseRender();
//	system("pause");
//	return 0;                                      
//}


class RealtimeRenderThread
{
public:
	RealtimeRenderThread(s32 width, s32 height, WNDPROC wndPROC)
	{
		mWidth = width;
		mHeight = height;
	}

	~RealtimeRenderThread()
	{
		std::cout << "Îö¹¹" << std::endl;
	}

	void operator()()
	{
		ls::lsWnd::lsWndInit(mWidth, mHeight, wndGUIProcHandler);
		ls::D3D11Renderer::initD3D11();

		d3dPreviewer = new ls::D3D11Renderer();
		ls::lsEnvironment::realtimeRenderer = d3dPreviewer;
#if 1
		MSG msg;
		ZeroMemory(&msg, sizeof(msg));
		ls::Timer timer;
		while (msg.message != WM_QUIT)
		{
			if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))

			{

				TranslateMessage(&msg);

				DispatchMessage(&msg);

				continue;

			}
			timer.tick();
			d3dPreviewer->newFrame(timer.deltaTime());
			d3dPreviewer->render();
		}
#endif

		
	}

private:
	ls::D3D11Renderer*  d3dPreviewer;
	s32 mWidth;
	s32 mHeight;

};





//int main()
//{
//	ls::lsWnd::lsWndInit(800, 600, wndGUIProcHandler);
//
//	ls::D3D11Previewer Previewer(800, 600,
//		ls::lsWnd::wnd.wndHwnd,
//		ls::lsWnd::wnd.wndHinstance,
//		ls::lsWnd::hw.d3dDevice,
//		ls::lsWnd::hw.d3dImmediateContext,
//		ls::lsWnd::hw.renderTargetView,
//		ls::lsWnd::hw.depthStencilView,
//		ls::lsWnd::hw.viewPort);
//
//
//
//
//
//
//
//	//jmxRGUILayout* layout = new SubdivideLayout();
//
//	////	std::cout << render.mTables[0]->samplesBuf.size() << std::endl;
//
//
//
//
//
//
//
//
//
//	MSG msg;
//
//	ZeroMemory(&msg, sizeof(msg));
//	ls::Timer timer;
//	while (msg.message != WM_QUIT)
//	{
//
//		if (PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
//
//		{
//
//			TranslateMessage(&msg);
//
//			DispatchMessage(&msg);
//
//			continue;
//
//		}
//
//		timer.tick();
//
//		Previewer.newFrame(timer.deltaTime());
//
//		
//		Previewer.render();
//
//
//
//
//
//
//		
//
//	}
//
//
//	system("pause");
//	return 0;
//}

#if 1
int main()
{
	ls::initRender();
#if 0
	ls_Enqueue_RenderCommand(TestRenderCommand)([]() {
		if (!ImGui::Begin("RenderCommand"))
		{
			ImGui::End();
		}
		ImGui::Button("Fuck");


		ImGui::End();
	});

	ls_Trigger_RenderCommand;
#endif
#if 1
	{
		auto package = ls::XMLParser::loadXMLFromMTSFile(PrePath,
			FileName);

		ls::ResourceManager::setPath(PrePath);
		auto scene = ls::ResourceManager::createSceneObj();
		scene->setSceneFromXML(PrePath, package);
		scene->render();
		ls::ResourceManager::write2File(scene->getMainFilm()->convert2Texture(),
			OutName);
		std::cout << "FINISHED!" << std::endl;
	}
#endif

	while (true)
	{

		Sleep(500);
	}

	ls::releaseRender();

	return 0;

}
#endif