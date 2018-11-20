#pragma once


#include<config/config.h>
#include<config/declaration.h>
namespace ls
{
	class Scene
	{
		friend ResourceManager;
	private:

		
		Scene();

	public:
		virtual ~Scene();

		void setScene(const std::string& path, XMLPackage& package);

	
		virtual bool intersect(ls_Param_In Ray& ray,
			ls_Param_Out Record* rec);

		virtual bool occlude(ls_Param_In const Ray& ray);

		virtual Light* envrionmentLight();

		virtual f32 sampleLight(ls_Param_In Sampler* sampler,
			ls_Param_Out Record* rec);

		virtual f32 sampleMesh(ls_Param_In Sampler* sampler,
			ls_Param_Out Record* rec);

		virtual s32 addMesh(Mesh* mesh);
		virtual void deleteMesh(Mesh* mesh);

		virtual s32 addLight(Light* light);
		virtual void deleteLight(Light* light);

		auto  getLightCount() { return mSceneLights.size(); }
		auto  getMeshCount() { return mSceneMeshs.size(); }



	protected:
		std::vector<Light*>  mSceneLights;
		std::vector<Mesh*>	 mSceneMeshs;
		Camera*				 mCamera;
		RenderAlgorithm*	 mAlgorithm;

		




	};
}

