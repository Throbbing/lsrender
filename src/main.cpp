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
	
#if 0
	auto package = ls::XMLParser::loadXMLFromMTSFile("G:\\Pro\\lsrender\\living-room\\", "scene.xml");

	auto scene = ls::ResourceManager::createSceneObj();

	ls::lsEmbree::initEmbree();

	scene->setScene("G:\\Pro\\lsrender\\living-room\\",package);

	scene->render();

	ls::lsEmbree::releaseEmbree();

#endif

	


	ls::Vec3 target = { 11.6408f, 14.3767f, 21.6714f };
	ls::Vec3 origin = { 12.0092f, 14.7441f, 22.5255f };
	ls::Vec3 up = { -0.166258f, 0.929834f, -0.328278f};

	auto c = ls::Transform::Mat4x4Camera(normalize(target - origin), up, ls::Point3(origin));

	DirectX::XMFLOAT4X4 cm(c(0, 0), c(0, 1), c(0, 2), c(0, 3),
		c(1, 0), c(1, 1), c(1, 2), c(1, 3),
		c(2, 0), c(2, 1), c(2, 2), c(2, 3),
		c(3, 0), c(3, 1), c(3, 2), c(3, 3));

	auto cmm = DirectX::XMLoadFloat4x4(&cm);

	DirectX::XMVECTOR scale, rotate, trans;
	DirectX::XMMatrixDecompose(&scale, &rotate, &trans,cmm);



	auto a = ls::Transform::Mat4x4Scale(20, 20, 1) * c;

	a = a.transpose();
	a.toString();




	

	system("pause");
	
	return 0;                                      
}