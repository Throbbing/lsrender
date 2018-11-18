#pragma once
#include<algorithm/renderalgorithm.h>
#include<spectrum/spectrum.h>
namespace ls
{
	class PathTracer:public RenderAlgorithm
	{
	public:
		PathTracer() {}
		virtual ~PathTracer() {}

		virtual void render(Scene* scene, Sampler* sampler,
			Camera* camera, RNG& rng) const override;

		virtual ls::Spectrum Li(ls_Param_In Record* cameraRec,
			ls_Param_In Scene* scene,
			ls_Param_In Sampler* sampler,
			ls_Param_In RNG& rng,
			ls_Param_In MemoryAllocater* arena)  const override;

	private:
		s32			mPathMaxDepth;
	};
}