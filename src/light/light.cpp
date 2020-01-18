#include<light/light.h>
#include<material/material.h>
ls::Light::Light(const std::string & id):Module(id)
{
	mDummyMaterial = new DummyMaterial;
}

ls::Light::~Light()
{
	delete mDummyMaterial;
}
