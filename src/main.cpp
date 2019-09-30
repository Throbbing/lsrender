#include<iostream>
#include<math/math.h>
#include<stdlib.h>
#include<embree3/rtcore.h>
#include<function/file.h>
#include<film/film.h>
#include<stdio.h>
#include<stdarg.h>
#include<cwchar>
#include<resource\xmlHelper.h>
#include<scene\scene.h>
#include<resource\resourceManager.h>
#include<config\common.h>
#include<function/timer.h>
#include<previewer\D3D11Previewer.h>

#define PrePath "SceneFile//veach-bidir//"
#define FileName "scene.xml"
#define OutName "veach_bidir.png"


extern LRESULT CALLBACK wndGUIProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);
//int main()
//{
//	ls::lsEmbree::initEmbree();
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
//	ls::lsEmbree::releaseEmbree();
//	system("pause");
//	return 0;                                      
//}


int main()
{
	ls::lsWnd::lsWndInit(800, 600, wndGUIProcHandler);

	ls::D3D11Previewer Previewer(800, 600,
		ls::lsWnd::wnd.wndHwnd,
		ls::lsWnd::wnd.wndHinstance,
		ls::lsWnd::hw.d3dDevice,
		ls::lsWnd::hw.d3dImmediateContext,
		ls::lsWnd::hw.renderTargetView,
		ls::lsWnd::hw.depthStencilView,
		ls::lsWnd::hw.viewPort);







	//jmxRGUILayout* layout = new SubdivideLayout();

	////	std::cout << render.mTables[0]->samplesBuf.size() << std::endl;









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

		Previewer.newFrame(timer.deltaTime());

		
		Previewer.render();






		

	}


	system("pause");
	return 0;
}