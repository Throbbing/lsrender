#pragma once

#include<algorithm/renderalgorithm.h>
#include<spectrum/spectrum.h>
namespace ls
{
	class BDPT :public RenderAlgorithm
	{
		friend ResourceManager;
	public:
		BDPT(s32 maxDepth,
			const std::string& id = "PathTracer") :RenderAlgorithm(id), mPathMaxDepth(maxDepth) {}
		virtual ~BDPT() {}

		virtual void render(ScenePtr scene,
			SceneRenderBlock* renderBlock,
			SamplerPtr sampler,
			CameraPtr camera, RNG& rng) const override;

		virtual RenderAlgorithmPtr copy() const override;
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
		BDPT(ParamSet& paramSet);
	private:
		s32			mPathMaxDepth;
	};
}