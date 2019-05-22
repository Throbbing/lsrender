#include <scatter/dielectric.h>
#include<function/func.h>
#include<math/frame.h>
#include<resource/xmlHelper.h>
#include<record/record.h>
#include<sampler/sampler.h>
void ls::Dielectric::sample(ls_Param_In Sampler * sampler, ls_Param_Out ScatteringRecord * rec)
{
	f32 u = sampler->next1D();

	f32 etaI = mEtaI;
	f32 etaT = mEtaT;

	f32 cosThetaI = Frame::cosTheta(rec->wo);
	bool entrying = true;
	if (cosThetaI < 0.f)
	{
		//入射方向在介质内部
		//即 入射光线是"离开"
		std::swap(etaI, etaT);
		cosThetaI = std::fabs(cosThetaI);
		entrying = false;
	}
	f32 cosThetaT = RenderLib::snellLaw(etaI, cosThetaI, etaT);


	//计算Fresnel
	auto fresnel = RenderLib::fresnelDielectric(cosThetaI, etaI, cosThetaT, etaT);

	//反射
	if (u < fresnel)
	{
		//由于是在局部空间，反射方向只需要对入射方向的xy取反即可
		rec->wi = Vec3(-rec->wo.x, -rec->wo.y, rec->wo.z);
		rec->pdf = fresnel;
		if (rec->transportMode == TransportMode::ETransport_Radiance)
			rec->pdfRadiance = rec->pdf;
		else
			rec->pdfImportance = rec->pdf;
		rec->scatterFlag = mSFlag & (~EScattering_Transmission);
		rec->sampledValue = Spectrum(fresnel / cosThetaI);
	}
	//折射
	else
	{
		Normal n = Normal(0.f, 0.f, 1.f);
		if (Frame::cosTheta(rec->wo) < 0.f) n = Normal(0.f, 0.f, -1.f);

		if (!lsMath::refract(n,
			rec->wo,
			etaI,
			etaT,
			&rec->wi))
		{
			//全反射
			rec->pdf = 0.f;
			rec->sampledValue = 0.f;
			return;
		}
		rec->pdf = 1.f - fresnel;
		if (rec->transportMode == TransportMode::ETransport_Radiance)
			rec->pdfRadiance = rec->pdf;
		else
			rec->pdfImportance = rec->pdf;

		rec->scatterFlag = mSFlag & (~EScattering_Reflection);

		Spectrum f = (Spectrum(1.f) - fresnel);
		if (rec->transportMode == ETransport_Radiance)
			f *= (etaI * etaI) / (etaT *etaT);

		rec->sampledValue = f / cosThetaI;



	}


}

f32 ls::Dielectric::pdf(ls_Param_In const Vec3 & wo)
{
	return 0.f;
}

ls::Spectrum ls::Dielectric::f(ls_Param_In const Vec3 & wi, ls_Param_In const Vec3 & wo)
{
	return Spectrum(0.f);
}

std::string ls::Dielectric::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Scatter: " << "Dielectric" << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::Dielectric::Dielectric(ParamSet & paramSet):ScatteringFunction(ScatteringFlag::EScattering_S
	| ScatteringFlag::EScattering_Reflection
	| ScatteringFlag::EScattering_Transmission
	| ScatteringFlag::EScattering_Surface)
{
	mEtaI = paramSet.queryf32("etaI",1.f);
	mEtaT = paramSet.queryf32("etaT", 1.414f);
}
