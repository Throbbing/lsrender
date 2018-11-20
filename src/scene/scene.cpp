#include<config\common.h>
#include<scene\scene.h>
#include<function\func.h>
#include<light\light.h>
#include<record\record.h>
#include<mesh\mesh.h>
#include<function\stru.h>

#include<camera\pinhole.h>
#include<algorithm\renderalgorithm.h>
#include<algorithm\visualNormal.h>
#include<resource\paramSet.h>
#include<resource\xmlHelper.h>
#include<resource\resourceManager.h>
ls::Scene::Scene()
{
	
}

ls::Scene::~Scene()
{
}

void ls::Scene::setScene(const std::string& path, XMLPackage & package)
{
	mAlgorithm = new VisualNormal();
	
	auto cameraParamSet = package.mParamSets[package.mCamera];

	for (auto & p : package.mShapes)
	{
		auto shapeParamSet = package.mParamSets[p.second];
		auto mesh = ResourceManager::loadMeshFromFile(path, shapeParamSet.queryString("filename"));
		mesh->applyTransform(shapeParamSet.queryTransform("toWorld"));
		mesh->commit();
		addMesh(mesh.get());

	}


}

bool ls::Scene::intersect(ls_Param_In Ray & ray, ls_Param_Out Record * rec)
{
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	RTCRayHit rtcHit;
	rtcHit.ray = GeometryLib::lsRay2Embree(ray);

	rtcIntersect1(lsEmbree::hw.rtcScene, &context, &rtcHit);

	if (rtcHit.hit.geomID == RTC_INVALID_GEOMETRY_ID)
		return false;

	RTCRecord rtcRecord;
	rtcRecord.rayHit = rtcHit;
	rtcRecord.geomID = rtcHit.hit.geomID;
	rtcRecord.primID = rtcHit.hit.primID;
	ray.tfar = rtcHit.ray.tfar;
	ray.tnear = rtcHit.ray.tnear;
	ray.time = rtcHit.ray.time;

	for (auto& mesh : mSceneMeshs)
	{
		if (mesh->intersect(ray, rtcRecord, rec))
		{
			return true;
		}
	}

	return false;
}

bool ls::Scene::occlude(ls_Param_In const Ray & ray)
{
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	
	auto rtcRay = GeometryLib::lsRay2Embree(ray);

	rtcOccluded1(lsEmbree::hw.rtcScene, &context, &rtcRay);

	if (rtcRay.tfar < 0.f)
		return true;

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
	mSceneMeshs.push_back(mesh);
	
	u32 id = rtcAttachGeometry(lsEmbree::hw.rtcScene, mesh->mEmbreeGem);
	mesh->mGeomID = id;
	return id;
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



