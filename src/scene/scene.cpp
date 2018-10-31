#include<scene/scene.h>
#include<function/func.h>
#include<light/light.h>

ls::Scene::Scene(const XMLPackage & xml)
{
	Unimplement;
}

ls::Scene::~Scene()
{
}

bool ls::Scene::intersect(ls_Param_In Ray & ray, ls_Param_Out Record * rec)
{
	Unimplement;
	return false;
}

bool ls::Scene::occlude(ls_Param_In const Ray & ray)
{
	Unimplement;
	return false;
}

ls::Light * ls::Scene::envrionmentLight()
{
	return nullptr;
}

f32 ls::Scene::sampleLight(ls_Param_In Sampler * sampler, ls_Param_Out Record * rec)
{
	Unimplement;
}

f32 ls::Scene::sampleMesh(ls_Param_In Sampler * sampler, ls_Param_Out Record * rec)
{
	Unimplement;
}

s32 ls::Scene::addMesh(Mesh * mesh)
{
	Unimplement;
	return s32();
}

void ls::Scene::deleteMesh(Mesh * mesh)
{
	Unimplement;
	
}

s32 ls::Scene::addLight(Light * light)
{
	Unimplement;
	return s32();
}

void ls::Scene::deleteLight(Light * light)
{
	Unimplement;
	
}



