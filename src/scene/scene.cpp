#include<config/common.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<algorithm/renderalgorithm.h>
#include<algorithm/visualNormal.h>
#include<algorithm/direct.h>
#include<algorithm/path.h>
#include<camera/pinhole.h>
#include<film/hdr.h>
#include<function/rng.h>
#include<function/stru.h>
#include<function/file.h>
#include<function/func.h>
#include<light/light.h>
#include<light/pointLight.h>
#include<mesh/mesh.h>
#include<material/matte.h>
#include<record/record.h>
#include<resource/paramSet.h>
#include<resource/xmlHelper.h>
#include<resource/resourceManager.h>
#include<scene/scene.h>
#include<sampler/randomsampler.h>
#include<texture/constantTexture.h>
ls::Scene::Scene()
{
	
}

ls::Scene::~Scene()
{
}

void ls::Scene::setSceneFromMTSXML(const ls::Path& path, XMLPackage & package)
{
	//¶ÁÈ¡äÖÈ¾Ëã·¨
	{
		
	}
	auto cameraParamSet = package.mParamSets[package.mCamera];

	for (auto & p : package.mShapes)
	{
		auto shapeParamSet = package.mParamSets[p.second];
		auto pathStr = path.str();
		auto mesh = ResourceManager::loadMeshFromFile(path, shapeParamSet.queryString("filename"));	
		if (mesh)
		{
			auto w = shapeParamSet.queryTransform("toWorld");
			mesh->applyTransform(w.getMat());
		}
		else
			continue;
		
		

		ParamSet bsdfSet = package.queryRefObject(shapeParamSet.getAllRefs(),
			EParamSet_BSDF);
		if (!bsdfSet.isValid())
			bsdfSet = shapeParamSet.queryParamSetByType("bsdf");

		if (!bsdfSet.isValid())
			ls_AssertMsg(false, "Invalid ParamSet: No BSDF in Shape");

		if (bsdfSet.getName() == "diffuse")
		{
			TexturePtr kr = new ConstantTexture(bsdfSet.querySpectrum("reflectance"));
			MaterialPtr matte = NewPtr<Matte>();
			((Matte*)matte)->applyReflectance(kr);
			mesh->applyMaterial(matte);
		}

		mesh->commit();
		addMesh(mesh.get());
	}

	{
		auto fov = cameraParamSet.queryf32("fov");
		auto world = cameraParamSet.queryTransform("toWorld");
		auto zNear = cameraParamSet.queryf32("nearClip",1e-2);
		auto zFar = cameraParamSet.queryf32("farClip",1e4);
		mCamera = new Pinhole(world.getMat()  ,
			0, 0, fov, zNear, zFar);

		
		

		auto filmParamSet = cameraParamSet.queryParamSetByType("film");
		auto width = filmParamSet.querys32("width");
		auto height = filmParamSet.querys32("height");

		auto film = new HDRFilm();
		film->setResolution(width, height);
		film->commit();

		mCamera->addFilm(film);
		mCamera->commit();


	}

	{
		for (auto& l : package.mLights)
		{
			auto lightParamSet = package.mParamSets[l.second];

			Point3 position = Point3(lightParamSet.queryVec3("position"));
			Spectrum intensity = lightParamSet.querySpectrum("intensity");

			LightPtr light = new PointLight(position,intensity);
			mSceneLights.push_back(light);

		}
	}

	rtcCommitScene(ls::lsEmbree::hw.rtcScene);
	


}

bool ls::Scene::intersect(ls_Param_In Ray & ray, ls_Param_Out IntersectionRecord * rec)
{
	RTCIntersectContext context;
	rtcInitIntersectContext(&context);

	RTCRayHit rtcHit;
	rtcHit.ray = GeometryLib::lsRay2Embree(ray);
	rtcHit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
	rtcIntersect1(lsEmbree::hw.rtcScene, &context, &rtcHit);

	auto ii = RTC_INVALID_GEOMETRY_ID;
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

f32 ls::Scene::sampleLight(ls_Param_In Sampler * sampler, ls_Param_Out LightSampleRecord * rec)
{
	rec->light = mSceneLights[0];
	return 1.f;
}

f32 ls::Scene::sampleMesh(ls_Param_In Sampler * sampler, ls_Param_Out MeshSampleRecord * rec)
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

void ls::Scene::render()
{
	RNG rng;
	SamplerPtr sampler = new RandomSampler();
	mAlgorithm->render(this, sampler,
		mCamera, rng);
}

ls::FilmPtr ls::Scene::getMainFilm() const
{
	return mCamera->getFilm();
}

std::string ls::Scene::strOut() const
{
	return "Scene";
}



