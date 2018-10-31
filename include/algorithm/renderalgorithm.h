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
			Camera* camera, RNG& rng) = 0;

		virtual ls::Spectrum Li(ls_Param_In Record* cameraRec,
			ls_Param_In Scene* scene,
			ls_Param_In Sampler* sampler,
			ls_Param_In RNG& rng,
			ls_Param_In MemoryAllocater* arena)  = 0;
	};


	
}
