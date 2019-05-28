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

#define PrePath "SceneFile//material-testball//"
#define FileName "scene.xml"
#define OutName "specularMetal.png"
int main()
{
	ls::lsEmbree::initEmbree();
	
//	ls::ResourceManager::loadTextureFromFileW("cbox", L"skylight-morn.exr");

	auto package = ls::XMLParser::loadXMLFromMTSFile(PrePath, 
		FileName);

//	ls::XMLParser::printXMLPackage(package);
	ls::ResourceManager::setPath(PrePath);
	auto scene = ls::ResourceManager::createSceneObj();

	scene->setSceneFromXML(PrePath, package);
	

	scene->render();

	ls::ResourceManager::write2File(scene->getMainFilm()->convert2Texture(),
		OutName);
	
	
	std::cout << "FINISHED!" << std::endl;



	
	ls::lsEmbree::releaseEmbree();
	system("pause");
	
	return 0;                                      
}