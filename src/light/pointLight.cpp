#include<function/func.h>
#include<light/pointLight.h>
#include<record/record.h>
#include<sampler/sampler.h>
#include<scatter/scatter.h>

void ls::PointLight::sample(ls_Param_In SamplerPtr sampler, ls_Param_Out LightSampleRecord * rec)
{
	auto uv = sampler->next2D();
	Vec3 wo;
	f32 pdf;
	MonteCarlo::sampleSphere(uv, &wo, &pdf);
	rec->samplePosition = mPosition;
	rec->sampleDirection = wo;	
	rec->le = mIntensity;
	rec->mode = EMeasure_SolidAngle;
	rec->pdfPos = 1.f;
	rec->pdfDir = pdf;
	rec->pdfW = pdf;
	rec->light = LightPtr(this);
}

void ls::PointLight::sample(ls_Param_In SamplerPtr sampler, ls_Param_In const IntersectionRecord * refRec, 
	ls_Param_Out LightSampleRecord * rec)
{
	Vec3 l2p = Vec3(refRec->position - mPosition);
	f32 dist = l2p.length();
	l2p /= dist;

	rec->samplePosition = mPosition;
	rec->sampleDirection = l2p;
	rec->le = mIntensity / (dist * dist);
	rec->mode = EMeasure_SolidAngle;
	rec->pdfPos = 1.f;
	rec->pdfDir = 1.f;
	rec->pdfW = 1.f;
	rec->light = LightPtr(this);
}

f32 ls::PointLight::pdf(ls_Param_In const LightSampleRecord * refRec)
{
	return 1.f;
}
