#include<scatter/lambertian.h>
#include<sampler/sampler.h>
#include<function/func.h>
#include<record/record.h>
void ls::Lambertian::sample(ls_Param_In Sampler * sampler, ls_Param_Out ScatteringRecord * rec)
{
	auto uv = sampler->next2D();

	MonteCarlo::sampleCosHemisphere(uv, &rec->wi, &rec->pdf);
	if (rec->transportMode == ETransport_Radiance)
		rec->pdfRadiance = rec->pdf;
	else
		rec->pdfImportance = rec->pdf;

	rec->scatterFlag = mSFlag;

}

f32 ls::Lambertian::pdf(ls_Param_In const Vec3& wi)
{
	return MonteCarlo::sampleCosHemispherePdf(wi);
}

ls::Spectrum ls::Lambertian::f(ls_Param_In const Vec3& wi,
	ls_Param_In const Vec3& wo)
{
	return lsMath::InvPi;
}
