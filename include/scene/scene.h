#pragma once


#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
#include<function/stru.h>
#include<d3d11.h>
namespace ls
{
	enum SceneFileType
	{
		EScene_MTS  = 0,
		EScene_UPBP ,
		EScene_PBRT ,
		EScene_LS   ,
		EScene_Unknown
	};
	struct SceneRenderBlock
	{
	public:
		s32 xStart;
		s32 yStart;
		s32 xEnd;
		s32 yEnd;
	};

	class SceneRenderTask
	{
	public:
		SceneRenderTask(s32 xStart,
			s32 yStart,
			s32 xEnd,
			s32 yEnd,
			SamplerPtr sampler,
			CameraPtr camera,
			RenderAlgorithmPtr algorithm,
			ScenePtr scene) :mXStart(xStart), mYStart(yStart), mXEnd(xEnd), mYEnd(yEnd), mSampler(sampler),mCamera(camera), mAlgorithm(algorithm), mScene(scene) {}

		void operator()();

	private:
		s32 mXStart;
		s32 mYStart;
		s32 mXEnd;
		s32 mYEnd;
		SamplerPtr mSampler;
		CameraPtr  mCamera;
		RenderAlgorithmPtr mAlgorithm;
		ScenePtr   mScene;
	};

	class Scene :public Module
	{
		friend ResourceManager;

	public:
		struct SampleInfo
		{
			s32 spp;
			s32 iterations;
			s32 directSample;
		};
	private:

		
		Scene(const std::string& id = "scene");

	public:
		virtual ~Scene();

		void setSceneFromXML(const Path& path, XMLPackage& package);

	
		virtual bool intersect(ls_Param_In Ray& ray,
			ls_Param_Out IntersectionRecord* rec);

		virtual bool occlude(ls_Param_In const Ray& ray);

		virtual LightPtr envrionmentLight();

		virtual f32 sampleLight(ls_Param_In Sampler* sampler,
			ls_Param_Out LightSampleRecord* rec);

		virtual f32 pdfSampleLight(ls_Param_In LightPtr light);

		virtual f32 sampleMesh(ls_Param_In Sampler* sampler,
			ls_Param_Out MeshSampleRecord* rec);

		virtual s32 addMesh(Mesh* mesh);
		virtual void deleteMesh(Mesh* mesh);

		virtual s32 addLight(Light* light);
		virtual void deleteLight(Light* light);

		virtual void render();

		auto  getLightCount() const { return mSceneLights.size(); }
		auto  getMeshCount()  const { return mSceneMeshs.size(); }
		auto  getWorldAABB()  const { return mWorldAABB; }

		FilmPtr getMainFilm() const;
		SceneFileType getSceneFileType() const { return mSceneFileType; }


		virtual void commit() override {}
		virtual std::string strOut() const override;



	protected:
		
		std::vector<LightPtr>	 mSceneLights;
		std::vector<MeshPtr>	 mSceneMeshs;
		CameraPtr				 mCamera = nullptr;
		RenderAlgorithmPtr	     mAlgorithm = nullptr;
		SamplerPtr				 mSampler = nullptr;
		LightPtr				 mEnvironmentLight = nullptr;
		AABB					 mWorldAABB;


		SceneFileType			 mSceneFileType = EScene_MTS;

		CanvasPtr				 mRealtimeCanvas = nullptr;


	};
}

