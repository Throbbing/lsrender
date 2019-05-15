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


int main()
{
	ls::lsEmbree::initEmbree();
	
	ls::ResourceManager::loadTextureFromFileW("cbox", L"skylight-morn.exr");

	auto package = ls::XMLParser::loadXMLFromMTSFile("cbox//", 
		"cbox.xml");

//	ls::XMLParser::printXMLPackage(package);

	auto scene = ls::ResourceManager::createSceneObj();

	scene->setScene("cbox//", package);
	

	scene->render();

	ls::ResourceManager::write2File(scene->getMainFilm()->convert2Texture(),
		"",
		"cbox.png");
	
	




	
	ls::lsEmbree::releaseEmbree();
	system("pause");
	
	return 0;                                      
}