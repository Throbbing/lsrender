#include<iostream>
#include<math/math.h>
#include<stdlib.h>
#include<embree3/rtcore.h>
#include<function/file.h>
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
	auto package = ls::XMLParser::loadXMLFromMTSFile("F://lsrender//cbox//", 
		"cbox.xml");

//	ls::XMLParser::printXMLPackage(package);

	auto scene = ls::ResourceManager::createSceneObj();

	scene->setScene("F://lsrender//cbox//", package);
	

	scene->render();

	




	
	ls::lsEmbree::releaseEmbree();
	system("pause");
	
	return 0;                                      
}