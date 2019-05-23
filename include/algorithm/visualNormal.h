#pragma once
#include<algorithm/renderalgorithm.h>

namespace ls
{
	class VisualNormal :public RenderAlgorithm
	{
		friend ResourceManager;
	public:
		VisualNormal():RenderAlgorithm("VisualNormal"){}
		virtual ~VisualNormal(){}

		virtual RenderAlgorithmPtr copy() const override { return new VisualNormal(); }

		virtual ls::Spectrum Li(ls_Param_In const DifferentialRay ray,
			ls_Param_In s32	depth,
			ls_Param_In CameraSampleRecord* cameraSampleRec,
			ls_Param_In ScenePtr scene,
			ls_Param_In SamplerPtr sampler,
			ls_Param_In RNG& rng,
			ls_Param_In MemoryAllocaterPtr arena) const;

		virtual void commit() override {}
		virtual std::string strOut() const override;

	protected:
		VisualNormal(ParamSet& paramSet);

	};
}