#include <scatter/conductor.h>
#include<function/func.h>
#include<math/frame.h>
#include<resource/xmlHelper.h>
#include<record/record.h>
#include<sampler/sampler.h>

void ls::Conductor::sample(ls_Param_In Sampler * sampler,
	ls_Param_In ls_Param_Out ScatteringRecord * rec)
{
	f32 cosThetaI = Frame::cosTheta(rec->wo);

	//conductor只产生反射光线
	//所以只处理半球空间上的入射光线
	if (cosThetaI <= 0.f)
	{
		rec->pdf = 0.f;
		rec->sampledValue = Spectrum(0.f);
		return;
	}
	
	Spectrum fresnel = RenderLib::fresnelConductor(cosThetaI, mEtaI, mEtaT, mK);

	//由于是在局部空间，反射方向只需要对入射方向的xy取反即可
	rec->wi = Vec3(-rec->wo.x, -rec->wo.y, rec->wo.z);
	rec->pdf = 1.f;
	if (rec->transportMode == ETransport_Radiance)
		rec->pdfRadiance = rec->pdf;
	else
		rec->pdfImportance = rec->pdf;
	rec->scatterFlag = mSFlag;

//	if (lsMath::closeZero(cosThetaI)) cosThetaI = lsMath::Epsilon;

	rec->sampledValue = fresnel / cosThetaI;


}

f32 ls::Conductor::pdf(ls_Param_In const Vec3& wi,
	ls_Param_In const Vec3 & wo)
{
	return 0.f;
}

ls::Spectrum ls::Conductor::f(ls_Param_In const Vec3 & wi, ls_Param_In const Vec3 & wo)
{
	return Spectrum(0.f);
}

std::string ls::Conductor::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Scatter: " << "Dielectric" << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::Conductor::Conductor(ParamSet & paramSet):ScatteringFunction(ScatteringFlag::EScattering_S
	| ScatteringFlag::EScattering_Reflection
	| ScatteringFlag::EScattering_Surface)
{
	mEtaI = paramSet.queryf32("etaI", 1.f);
	mEtaT = paramSet.querySpectrum("etaT", 1.5f);
	mK = paramSet.querySpectrum("k", 1.f);
}
