#pragma once
#include<algorithm\renderalgorithm.h>

namespace ls
{
	class VisualNormal :public RenderAlgorithm
	{
	public:
		VisualNormal() {}
		virtual ~VisualNormal(){}



		virtual ls::Spectrum Li(ls_Param_In const DifferentialRay ray,
			ls_Param_In s32	depth,
			ls_Param_In CameraSampleRecord* cameraSampleRec,
			ls_Param_In ScenePtr scene,
			ls_Param_In SamplerPtr sampler,
			ls_Param_In RNG& rng,
			ls_Param_In MemoryAllocaterPtr arena) const;

		virtual void commit() override {}

	};
}