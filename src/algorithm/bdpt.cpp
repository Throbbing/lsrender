#include<config/lsPtr.h>
#include<algorithm/bdpt.h>
#include<record/record.h>
#include<function/stru.h>
#include<scene/scene.h>
#include<light/light.h>
#include<material/material.h>
#include<resource/xmlHelper.h>
#include<resource/resourceManager.h>
#include<sampler/sampler.h>
#include<spectrum/spectrum.h>
#include<scatter/scatter.h>


void ls::BDPT::render(ScenePtr scene, SceneRenderBlock * renderBlock, SamplerPtr sampler, CameraPtr camera, RNG & rng) const
{
}

ls::RenderAlgorithmPtr ls::BDPT::copy() const
{
	return RenderAlgorithmPtr();
}

ls::Spectrum ls::BDPT::Li(ls_Param_In const DifferentialRay ray, 
	ls_Param_In s32 depth, 
	ls_Param_In CameraSampleRecord * cameraSampleRec, ls_Param_In Scene * scene, ls_Param_In Sampler * sampler, ls_Param_In RNG & rng, ls_Param_In MemoryAllocater * arena) const
{
	return ls::Spectrum();
}

std::string ls::BDPT::strOut() const
{
	return std::string();
}

ls::BDPT::BDPT(ParamSet & paramSet)
{
}
