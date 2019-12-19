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
#include<function/timer.h>
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
#include<thread/AsynTask.h>
#include<thread/thread.h>
#include<thread/QueuedThreadPool.h>

#include<realtime/D3D11Realtime.h>
#include<realtime/D3D11Canvas.h>

ID3D11ShaderResourceView* finalSRV = nullptr;
ID3D11Texture2D*		  finalTex = nullptr;
void ls::SceneRenderTask::operator ()()
{
	RNG rng;
	SceneRenderBlock block;
	block.xStart = mXStart;
	block.yStart = mYStart;
	block.xEnd = mXEnd;
	block.yEnd = mYEnd;

	mAlgorithm->render(mScene, &block, mSampler, mCamera, rng);
}



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
//		mAlgorithm = new VisualNormal();
		mAlgorithm->commit();
	}

	//读取采样器
	{
		mSampler = ResourceManager::createSampler(package.queryParamSetByClass("sampler")[0]);
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

			if (t->getAreaLight())
				addLight(t->getAreaLight());
		}
		
	}

	//采样信息
	{
		lsRender::sampleInfo.spp = package.mSampleInfo.querys32("spp",1);
		lsRender::sampleInfo.iterations = package.mSampleInfo.querys32("iterations", 1);
		lsRender::sampleInfo.directSamples = package.mSampleInfo.querys32("directSamples", 1);
	}

	rtcCommitScene(ls::lsEmbree::hw.rtcScene);
	
	for (auto light : mSceneLights)
	{
		if (light->getLightType() == ELight_Environment)
		{
			mEnvironmentLight = light;
			break;
		}
	}

	if (!mSceneMeshs.empty()) mWorldAABB = mSceneMeshs[0]->getAABB();

	for (s32 i = 1; i < mSceneMeshs.size(); ++i)
	{
		mWorldAABB.unionAABB(mSceneMeshs[i]->getAABB());
	}


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

ls::LightPtr ls::Scene::envrionmentLight()
{
	return mEnvironmentLight;
}

f32 ls::Scene::sampleLight(ls_Param_In Sampler * sampler, ls_Param_Out LightSampleRecord * rec)
{
	f32 u = sampler->next1D();
	auto selectedIndex = std::min((u32)(mSceneLights.size() * u), (u32)mSceneLights.size() - 1);
	rec->light = mSceneLights[selectedIndex];

	return 1.f / mSceneLights.size();
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


struct RenderTaskWrap
{
	s32 xStart, xEnd, yStart, yEnd;
	ls::CameraPtr camera;
	ls::ThreadTaskPtr task;
};

void ls::Scene::render()
{

	{
		auto width = mCamera->getFilm()->getWidth();
		auto height = mCamera->getFilm()->getHeight();
		auto ppCanvas = &mRealtimeCanvas;

		ls_Enqueue_OnceRenderCommand(CreateRealtimeCanvas)([ppCanvas,width,height]()
		{
			*ppCanvas = new D3D11Canvas2D(width, height, nullptr);
		});

		ls_Enqueue_RenderCommand(RenderResultWindow)([ppCanvas,width,height]() 
		{
			if (!ImGui::Begin("RenderWindow"))
			{
				ImGui::End();
				return;
			}
			ImGui::BeginChild("Result", ImVec2(width + 2, height + 2));
			ImGui::Image((*ppCanvas)->getHardwareResource(),ImVec2(width,height));
			ImGui::EndChild();

			ImGui::End();
		});


		ls_Trigger_RenderCommand;

		
	}
	ls::Log::log("Start Render ! \n");
	auto threadUsedCount = std::thread::hardware_concurrency();
	u32 hasProcess = 0;
	u32 allProcess = mCamera->getFilm()->getWidth() * mCamera->getFilm()->getHeight() * lsRender::sampleInfo.iterations;


	

	std::vector<RenderTaskWrap> renderTasks;


	auto screenWidth = mCamera->getFilm()->getWidth();
	auto screenHeight = mCamera->getFilm()->getHeight();

	int blockResX = screenWidth / lsRender::sceneRenderBlockInfo.blockSizeX;
	int blockResY = screenHeight / lsRender::sceneRenderBlockInfo.blockSizeY;
	if (blockResX * lsRender::sceneRenderBlockInfo.blockSizeX <
		screenWidth)
		blockResX += 1;
	if (blockResY * lsRender::sceneRenderBlockInfo.blockSizeY <
		screenHeight)
		blockResY += 1;

	s32 taskSize = blockResX * blockResY;
	std::vector<RenderAlgorithmPtr> renderAlgorithms(taskSize);
	std::vector<FilmPtr>		    films(taskSize);
	std::vector<SamplerPtr>			samplers(taskSize);
	std::vector<CameraPtr>			cameras(taskSize);

	for (s32 i = 0; i < taskSize; ++i)
	{
		renderAlgorithms[i] = mAlgorithm->copy(); renderAlgorithms[i]->commit();
		films[i] = mCamera->getFilm()->copy(); films[i]->commit();
		samplers[i] = mSampler->copy(); samplers[i]->commit();
		cameras[i] = mCamera->copy(); cameras[i]->addFilm(films[i]); cameras[i]->commit();
	}

	for (s32 blockY = 0; blockY < blockResY; ++blockY)
	{
		for (s32 blockX = 0; blockX < blockResX; ++blockX)
		{

			s32 xStart = blockX * lsRender::sceneRenderBlockInfo.blockSizeX;
			s32 xEnd = std::min(xStart + lsRender::sceneRenderBlockInfo.blockSizeX,
				screenWidth);

			s32 yStart = blockY * lsRender::sceneRenderBlockInfo.blockSizeY;
			s32 yEnd = std::min(yStart + lsRender::sceneRenderBlockInfo.blockSizeY,
				screenHeight);

			s32 index = blockY * blockResX + blockX;
			ls::ThreadTaskPtr task = new ls::AsynOnceTask<SceneRenderTask>(
				xStart,
				yStart,
				xEnd,
				yEnd,
				samplers[index],
				cameras[index],
				renderAlgorithms[index],
				this);

			RenderTaskWrap wrap;
			wrap.task = task;
			wrap.xStart = xStart;
			wrap.yStart = yStart;
			wrap.xEnd = xEnd;
			wrap.yEnd = yEnd;
			wrap.camera = cameras[index];
			renderTasks.push_back(wrap);
		}
	}



	Timer timer;
	timer.tick();

	for (auto& task : renderTasks)
		lsEnvironment::renderThreadPool->addTask(task.task);

	std::vector<RenderTaskWrap> undoTasks(renderTasks.begin(), renderTasks.end());
	std::vector<RenderTaskWrap> temp;
	
	while (true)
	{
		int finishTask = 0;
		for (auto& task : undoTasks)
		{
			if (task.task->getThreadTaskState() == EThreadTaskState_DONE)
			{
				// 该 Block 渲染完成
				// 添加进实时列表
				finishTask++;
				s32 xStart = task.xStart;
				s32 xEnd = task.xEnd;
				s32 quadWidth = xEnd - xStart;
				s32 yStart = task.yStart;
				s32 yEnd = task.yEnd;
				s32 quadHeight = yEnd - yStart;

				std::vector<Vec4> data(quadWidth * quadHeight);
				task.camera->getFilm()->flush();
				auto* rawData = (Spectrum*)task.camera->getFilm()->convert2Texture()->getRawData();
				
				auto width = task.camera->getFilm()->getWidth();
				auto height = task.camera->getFilm()->getHeight();

				for (s32 y = yStart; y < yEnd; ++y)
				{
					for (s32 x = xStart; x < xEnd; ++x)
					{
						auto color = rawData[y * width + x];
						f32 rgb[3];
						color.toSRGB(rgb);
						data[(y - yStart) * quadWidth + (x - xStart)] = Vec4(rgb[0],rgb[1],rgb[2],1.f);
					}
				}
				auto ppCanvas = &mRealtimeCanvas;

				ls_Enqueue_AnonymousOnceRenderCommand([xStart, yStart, xEnd, yEnd,
					data,ppCanvas]() 
				{
					auto canvas = (*ppCanvas);

					canvas->drawQuad(xStart, xEnd, yStart, yEnd, (void*)&data[0],
						ECanvasOpAccess_CPU);
				});

				ls_Trigger_RenderCommand;

			}
			else
			{
				// 临时保存未完成的任务
				temp.push_back(task);
			}
		}
		if (undoTasks.empty())
			break;

		// 
		undoTasks = temp;
		temp.clear();

		Sleep(1000);
		Log::log("%f \n", float(finishTask) / float(renderTasks.size()));
	}
	
	timer.tick();

	auto renderTime = timer.deltaTime();

	ls::Log::log("Render Time: %f s ! \n", renderTime);

#if 0
	u32 startThread = std::min(threadUsedCount, (u32)renderBlocks.size());
	u32 taskIndex = startThread;
	u32 taskCount = renderBlocks.size();

	ThreadPool threadPool;
	std::vector<std::shared_ptr<ls::ThreadWaker>> wakers;

	for (u32 i = 0; i < startThread; ++i)
	{
		renderBlocks[i].sampler = samplers[i];
		renderBlocks[i].algorithm = renderAlgorithms[i];
		renderBlocks[i].camera = mCamera;
		renderBlocks[i].scene = this;
		wakers.push_back(threadPool.addTask(std::bind(&SceneRenderBlock::run, &renderBlocks[i])));
		wakers.back()->wake();
	}

	//遍历所有线程，如果有线程完成工作，就分配给其一个新的任务
	
	while (taskIndex < taskCount)
	{
		for (u32 i = 0; i < threadUsedCount; ++i)
		{
			auto& taskThread = threadPool.mTasks[i];
			if (taskThread->go())
			{
				if (taskIndex >= taskCount)
					continue;
				//赋予新任务
				renderBlocks[taskIndex].sampler = renderBlocks[i].sampler;
				renderBlocks[taskIndex].algorithm = renderBlocks[i].algorithm;
				renderBlocks[taskIndex].camera = renderBlocks[i].camera;
				renderBlocks[taskIndex].scene = renderBlocks[i].scene;


				taskThread->mTask = std::bind(&SceneRenderBlock::run, &renderBlocks[taskIndex]);

				//再次唤醒线程
				wakers[i]->wake();
				//任务数+1
				++taskIndex;

				hasProcess += (renderBlocks[i].xEnd - renderBlocks[i].xStart) *
					(renderBlocks[i].yEnd - renderBlocks[i].yStart) *
					lsRender::sampleInfo.iterations;

				f32 renderPercent = (f32)hasProcess / (f32)allProcess;

				std::cout << renderPercent << std::endl;
				
			}
			else
			{
				Sleep(50);
			}
		}
	}

	//所有任务都分配完成，等待最后线程任务完成
	for (u32 i = 0; i < startThread; ++i)
	{
		while (!threadPool.go(i))
		{
			Sleep(50);

		}
	}
#endif
	ls::Log::log("Merge Multi-Block Film ! \n");
	mCamera->getFilm()->merge(films);

//	mCamera->getFilm()->flush();

	
	
}

ls::FilmPtr ls::Scene::getMainFilm() const
{
	return mCamera->getFilm();
}

std::string ls::Scene::strOut() const
{
	return "Scene";
}


