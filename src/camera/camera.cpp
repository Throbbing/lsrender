#include<camera/camera.h>
#include<material/material.h>
ls::Camera::Camera(const std::string & id):Module(id)
{
	mMaterial = new DummyMaterial();
}

ls::Camera::~Camera()
{
	delete mMaterial;
}
