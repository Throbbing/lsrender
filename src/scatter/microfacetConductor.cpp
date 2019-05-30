#include<scatter/microfacetConductor.h>
#include<function/func.h>
#include<math/frame.h>
#include<resource/xmlHelper.h>
#include<record/record.h>
#include<sampler/sampler.h>

void ls::MicrofacetConductor::sample(ls_Param_In Sampler * sampler, 
	ls_Param_In ls_Param_Out ScatteringRecord * rec)
{
	/*
		fr = D(wh) * Fr(wi,wo) * G(wi,wo) / (4 * cosThetaI * cosThetaO)
	*/
	f32 cosThetaI = Frame::cosTheta(rec -> wo);
	//采样D
	Distribution distribution(mAlphaU, mAlphaV, mDistributionType, mSampleAll);
	
	f32 pdfH;
	Vec3 wh;
	f32 D = distribution.sample(sampler, rec->wo, &wh, &pdfH);

	if (pdfH == 0.f)
	{
		rec->pdf = 0.f;
		rec->sampledValue = Spectrum(0.f);
		return;
	}
	
	//计算出射方向
	rec->wi = 2 * dot(rec->wo, wh) * wh - rec->wo;
	f32 cosThetaH = dot(rec->wi, wh);

	if (cosThetaH <= 0.f)
	{
		rec->pdf = 0.f;
		rec->sampledValue = Spectrum(0.f);
		return;
	}
	rec->pdf = pdfH / (4.f * cosThetaH);
	if (rec->transportMode == ETransport_Radiance)
		rec->pdfRadiance = rec->pdf;
	else
		rec->pdfImportance = rec->pdf;
	rec->scatterFlag = mSFlag;
	
	//计算Fresnel
	auto fresnel = RenderLib::fresnelConductor(dot(rec->wo,wh),
		mEtaI, mEtaT, mK);

	//计算G
	f32 G = distribution.G(rec->wi, rec->wo, wh);
	rec->sampledValue = D * fresnel * G / (4 * Frame::absCosTheta(rec->wi) * Frame::absCosTheta(rec->wo));
}

f32 ls::MicrofacetConductor::pdf(ls_Param_In const Vec3 & wo)
{
	return f32();
}

ls::Spectrum ls::MicrofacetConductor::f(ls_Param_In const Vec3 & wi, ls_Param_In const Vec3 & wo)
{
	return ls::Spectrum();
}

std::string ls::MicrofacetConductor::strOut() const
{
	return std::string();
}

ls::MicrofacetConductor::MicrofacetConductor(ParamSet & paramSet):ScatteringFunction(ScatteringFlag::EScattering_G
	| ScatteringFlag::EScattering_Reflection
	| ScatteringFlag::EScattering_Surface)
{
}
