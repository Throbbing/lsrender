#pragma once
#include<config/config.h>
#include<config/common.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<algorithm/renderalgorithm.h>


namespace ls
{
	class DirectTracer :public RenderAlgorithm
	{
		friend ResourceManager;

	public:
		DirectTracer(s32 maxDepth,
			s32 rouletteDepth):
			mMaxDepth(maxDepth),
			mRouletteDepth(rouletteDepth)
		{

		}

		
		virtual ~DirectTracer() {}


		virtual ls::Spectrum Li(ls_Param_In const DifferentialRay ray,
			ls_Param_In s32					depth,
			ls_Param_In CameraSampleRecord* cameraSampleRec,
			ls_Param_In ScenePtr scene,
			ls_Param_In SamplerPtr sampler,
			ls_Param_In RNG& rng,
			ls_Param_In MemoryAllocaterPtr arena) const override;


		virtual void commit() override {}
		virtual std::string strOut() const override;

	protected:
		DirectTracer(ParamSet& paramSet);
		

	protected:
		s32					mMaxDepth;
		s32					mRouletteDepth;
	};
}
