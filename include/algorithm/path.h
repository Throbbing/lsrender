#pragma once
#include<algorithm/renderalgorithm.h>
#include<spectrum/spectrum.h>
namespace ls
{
	class PathTracer:public RenderAlgorithm
	{
		friend ResourceManager;
	public:
		PathTracer(s32 maxDepth):mPathMaxDepth(maxDepth){}
		virtual ~PathTracer() {}


		virtual ls::Spectrum Li(ls_Param_In const DifferentialRay ray,
			ls_Param_In s32	depth,
			ls_Param_In CameraSampleRecord* cameraSampleRec,
			ls_Param_In Scene* scene,
			ls_Param_In Sampler* sampler,
			ls_Param_In RNG& rng,
			ls_Param_In MemoryAllocater* arena)  const override;

		virtual void commit() override {}

		virtual std::string strOut() const override;

	protected:
		PathTracer(ParamSet& paramSet);
	private:
		s32			mPathMaxDepth;
	};
}