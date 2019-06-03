#include<scatter/lambertian.h>
#include<sampler/sampler.h>
#include<math/frame.h>
#include<function/func.h>
#include<record/record.h>
#include<resource/xmlHelper.h>
void ls::Lambertian::sample(ls_Param_In Sampler * sampler, 
	ls_Param_In ls_Param_Out ScatteringRecord * rec)
{
	auto uv = sampler->next2D();

	MonteCarlo::sampleCosHemisphere(uv, &rec->wi, &rec->pdf);
	if (rec->transportMode == ETransport_Radiance)
		rec->pdfRadiance = rec->pdf;
	else
		rec->pdfImportance = rec->pdf;


	rec->scatterFlag = mSFlag;
	rec->sampledValue = lsMath::InvPi;

}

f32 ls::Lambertian::pdf(ls_Param_In const Vec3& wi,
	ls_Param_In const Vec3& wo)
{
	if (!Frame::hemisphere(wi, wo))
		return 0.f;
	return MonteCarlo::sampleCosHemispherePdf(wi);
}

ls::Spectrum ls::Lambertian::f(ls_Param_In const Vec3& wi,
	ls_Param_In const Vec3& wo)
{
	return lsMath::InvPi;
}

std::string ls::Lambertian::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Scatter: " << "Lambertian" << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}



ls::Lambertian::Lambertian(ParamSet& paramSet) :ScatteringFunction(ScatteringFlag::EScattering_D
	| ScatteringFlag::EScattering_Reflection
	| ScatteringFlag::EScattering_Surface)
{

}