#include <algorithm\visualNormal.h>
#include<record/record.h>
#include<function/stru.h>
#include<scene/scene.h>
#include<light/light.h>
#include<sampler/sampler.h>
#include<spectrum/spectrum.h>
#include<scatter/scatter.h>


ls::Spectrum ls::VisualNormal::Li(ls_Param_In Record * cameraRec, 
	ls_Param_In Scene * scene, 
	ls_Param_In Sampler * sampler, 
	ls_Param_In RNG & rng, 
	ls_Param_In MemoryAllocater * arena) const
{
	
	auto cr = CameraSamplePtrCast(cameraRec);

	DifferentialRay queryRay = DifferentialRay(cr->samplePosition,
		cr->sampleDirection, 0,
		cr->time);

	IntersectionRecord itsRec;
	if (scene->intersect(queryRay, &itsRec))
	{
		auto offsetNormal = itsRec.ns * 0.5f + 0.5f;
		return Spectrum(offsetNormal.x, offsetNormal.y, offsetNormal.z);
//		return Spectrum(itsRec.position.z / 100.f);
//		return Spectrum(1, 0, 0);
	}
	
	return Spectrum(0.f);
}
