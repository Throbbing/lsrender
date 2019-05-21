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
#include<thread/thread.h>
ls::Scene::Scene(const std::string& id):Module(id)
{
	
}

ls::Scene::~Scene()
{
}

void ls::Scene::setSceneFromXML(const ls::Path& path, XMLPackage & package)
{
	//读取渲染算法
	{
		mAlgorithm = ResourceManager::createAlgorithm(package.mParamSets[package.mIntegrator]);
		mAlgorithm->commit();
	}

	//读取采样器
	{
		mSampler = ResourceManager::createSampler(package.queryParamSetByType("sampler"));
		mSampler->commit();
	}

	//相机
	{
		mCamera = ResourceManager::createCamera(package.mParamSets[package.mCamera]);
		mCamera->commit();
	}

	//光源
	for(auto& lightIndex:package.mLights)
	{
		auto t = ResourceManager::createLight(package.mParamSets[lightIndex.second]);
		t->commit();

		addLight(t);
	}

	//模型
	for (auto& meshIndex : package.mShapes)
	{
		auto meshs = ResourceManager::createMesh(package.mParamSets[meshIndex.second]);
		for (auto t : meshs)
		{
			t->commit();
			addMesh(t);
		}
		
	}

	//采样信息
	{
		lsRender::sampleInfo.spp = package.mSampleInfo.querys32("spp",1);
		lsRender::sampleInfo.iterations = package.mSampleInfo.querys32("iterations", 1);
		lsRender::sampleInfo.directSamples = package.mSampleInfo.querys32("direcSamples", 1);
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
	s32 id = mSceneLights.size();
	mSceneLights.push_back(light);
	return id;
}

void ls::Scene::deleteLight(Light * light)
{
	Unimplement;
	
}

void ls::Scene::render()
{
	auto threadUsedCount = std::thread::hardware_concurrency();

	std::vector<RenderAlgorithmPtr> renderAlgorithms(threadUsedCount);
	std::vector<FilmPtr>		    films(threadUsedCount);
	std::vector<SamplerPtr>			samplers(threadUsedCount);

	renderAlgorithms[0] = mAlgorithm;
	films[0] = mCamera->getFilm();
	samplers[0] = mSampler;

	for (s32 i = 1; i < threadUsedCount; ++i)
	{
		renderAlgorithms[i] = renderAlgorithms[0]->copy();
		films[i] = films[0]->copy();
		samplers[i] = samplers[0]->copy();
	}

	std::vector<SceneRenderBlock> renderBlocks;
	auto screenWidth = films[0]->getWidth();
	auto screenHeight = films[0]->getHeight();

	int blockResX = screenWidth / lsRender::sceneRenderBlockInfo.blockSizeX;
	int blockResY = screenHeight / lsRender::sceneRenderBlockInfo.blockSizeY;
	if (blockResX * lsRender::sceneRenderBlockInfo.blockSizeX <
		screenWidth)
		blockResX += 1;
	if (blockResY * lsRender::sceneRenderBlockInfo.blockSizeY <
		screenHeight)
		blockResY += 1;

	for (s32 blockY = 0; blockY < blockResY; ++blockY)
	{
		for (s32 blockX = 0; blockX < blockResX; ++blockX)
		{
			SceneRenderBlock scr;
			scr.xStart = blockX * lsRender::sceneRenderBlockInfo.blockSizeX;
			scr.xEnd = std::min(scr.xStart + lsRender::sceneRenderBlockInfo.blockSizeX,
				screenWidth);

			scr.yStart = blockY * lsRender::sceneRenderBlockInfo.blockSizeY;
			scr.yEnd = std::min(scr.yStart + lsRender::sceneRenderBlockInfo.blockSizeY,
				screenHeight);

			renderBlocks.push_back(scr);
		}
	}
	

	
	
	
	RNG rng;
	
}

ls::FilmPtr ls::Scene::getMainFilm() const
{
	return mCamera->getFilm();
}

std::string ls::Scene::strOut() const
{
	return "Scene";
}



