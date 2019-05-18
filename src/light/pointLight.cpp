#include<function/func.h>
#include<light/pointLight.h>
#include<record/record.h>
#include<resource/xmlHelper.h>
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

std::string ls::PointLight::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Light: " << "Point" << std::endl;
	oss << "Intensity: " << std::endl;
    oss << mIntensity.toString() << std::endl;
	oss << "Position: " << std::endl;
	oss << mPosition.toString() << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::PointLight::PointLight(ParamSet & paramSet)
{
	mPosition = Point3(paramSet.queryVec3("position"));
	mIntensity = paramSet.querySpectrum("intensity");
}
