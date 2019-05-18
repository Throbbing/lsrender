#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
#include<spectrum/spectrum.h>
namespace ls
{
	class RenderAlgorithm :public Module
	{
	public:
		RenderAlgorithm(){}
		virtual ~RenderAlgorithm() {}

		virtual void render(ScenePtr scene, SamplerPtr sampler,
			CameraPtr camera, RNG& rng) const ;

		virtual ls::Spectrum Li(ls_Param_In const DifferentialRay ray,
			ls_Param_In s32	depth,
			ls_Param_In CameraSampleRecord* cameraSampleRec,
			ls_Param_In ScenePtr scene,
			ls_Param_In SamplerPtr sampler,
			ls_Param_In RNG& rng,
			ls_Param_In MemoryAllocaterPtr arena) const = 0;




	};


	
}
