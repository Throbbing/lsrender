#include<iostream>
#include<math/math.h>
#include<stdlib.h>
#include<embree3/rtcore.h>

#include<stdio.h>
#include<stdarg.h>
#include<cwchar>
#include<resource\xmlHelper.h>
#include<scene\scene.h>
#include<resource\resourceManager.h>
#include<config\common.h>


int main()
{
	

	auto package = ls::XMLParser::loadXMLFromMTSFile("G:\\Pro\\lsrender\\dragon\\", "scene.xml");

	auto scene = ls::ResourceManager::createSceneObj();

	ls::lsEmbree::initEmbree();

	scene->setScene("G:\\Pro\\lsrender\\dragon\\",package);

	scene->render();

	ls::lsEmbree::releaseEmbree();
	system("pause");
	
	return 0;                                      
}