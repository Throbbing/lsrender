#pragma once
#include<algorithm\renderalgorithm.h>

namespace ls
{
	class VisualNormal :public RenderAlgorithm
	{
	public:
		VisualNormal() {}
		virtual ~VisualNormal(){}



		virtual ls::Spectrum Li(ls_Param_In Record* cameraRec,
			ls_Param_In Scene* scene,
			ls_Param_In Sampler* sampler,
			ls_Param_In RNG& rng,
			ls_Param_In MemoryAllocater* arena) const;

	};
}