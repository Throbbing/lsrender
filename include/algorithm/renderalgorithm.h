#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<spectrum/spectrum.h>
namespace ls
{
	class RenderAlgorithm
	{
	public:
		RenderAlgorithm(){}
		virtual ~RenderAlgorithm() {}

		virtual void render(Scene* scene, Sampler* sampler,
			Camera* camera, RNG& rng)const ;

		virtual ls::Spectrum Li(ls_Param_In const DifferentialRay ray,
			ls_Param_In CameraSampleRecord* cameraSampleRec,
			ls_Param_In Scene* scene,
			ls_Param_In Sampler* sampler,
			ls_Param_In RNG& rng,
			ls_Param_In MemoryAllocater* arena) const = 0;
	};


	
}
