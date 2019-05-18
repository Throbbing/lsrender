#include <algorithm\visualNormal.h>
#include<record/record.h>
#include<function/stru.h>
#include<scene/scene.h>
#include<light/light.h>
#include<resource/resourceManager.h>
#include<sampler/sampler.h>
#include<spectrum/spectrum.h>
#include<scatter/scatter.h>


ls::Spectrum ls::VisualNormal::Li(
	ls_Param_In const DifferentialRay ray,
	ls_Param_In s32	depth,
	ls_Param_In CameraSampleRecord* cameraSampleRec,
	ls_Param_In ScenePtr scene,
	ls_Param_In SamplerPtr sampler,
	ls_Param_In RNG& rng,
	ls_Param_In MemoryAllocaterPtr arena) const
{
	
	auto queryRay = ray;

	IntersectionRecord itsRec;
	if (scene->intersect(queryRay, &itsRec))
	{
		auto offsetNormal = itsRec.ns * 0.5f + 0.5f;
		return Spectrum(offsetNormal.x, offsetNormal.y, offsetNormal.z);
//		return Spectrum(1, 0, 0);

	}
	return Spectrum(0.f);
}

std::string ls::VisualNormal::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Render Algorithm: " << "Visual Normal" << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::VisualNormal::VisualNormal(ParamSet & paramSet)
{
}
