#pragma once

#include<scatter/scatter.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<scatter/distribution.h>
namespace ls
{

	class MicrofacetConductor :public ScatteringFunction
	{
		friend ResourceManager;
	public:
		MicrofacetConductor(f32 etaI, Spectrum etaT, Spectrum k,
			f32 alphaU,
			f32 alphaV) :
			ScatteringFunction(ScatteringFlag::EScattering_G
				| ScatteringFlag::EScattering_Reflection
				| ScatteringFlag::EScattering_Surface)
		{
			mEtaI = etaI;
			mEtaT = etaT;
			mK = k;
			mAlphaU = alphaU;
			mAlphaV = alphaV;
			mDistributionType = EDistribution_Beckman;
			mSampleAll = true;
		}


		virtual bool isDelta() { return false; }

		virtual void sample(ls_Param_In Sampler* sampler,
			ls_Param_In ls_Param_Out ScatteringRecord* rec) override;

		virtual f32 pdf(ls_Param_In const Vec3& wo) override;
		virtual ls::Spectrum f(ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wo);

		virtual void commit() override {};
		virtual std::string strOut() const override;

	protected:
		MicrofacetConductor(ParamSet& paramSet);

	private:
		f32					mEtaI;
		Spectrum			mEtaT;
		Spectrum			mK;
		f32					mAlphaU;
		f32					mAlphaV;
		DistributionType	mDistributionType;
		bool				mSampleAll;




	};
}