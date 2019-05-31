#pragma once

#pragma once
#include<scatter/scatter.h>
#include<config/declaration.h>
#include<config/lsPtr.h>

namespace ls
{

	class Conductor :public ScatteringFunction
	{
		friend ResourceManager;
	public:
		Conductor(f32 etaI, Spectrum etaT,Spectrum k) :
			ScatteringFunction(ScatteringFlag::EScattering_S
				| ScatteringFlag::EScattering_Reflection
				| ScatteringFlag::EScattering_Surface)
		{
			mEtaI = etaI;
			mEtaT = etaT;
			mK = k;
		}


		virtual bool isDelta() { return true; }

		virtual void sample(ls_Param_In Sampler* sampler,
			ls_Param_In ls_Param_Out ScatteringRecord* rec) override;

		virtual f32 pdf(ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wo) override;
		virtual ls::Spectrum f(ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wo);

		virtual void commit() override {};
		virtual std::string strOut() const override;

	protected:
		Conductor(ParamSet& paramSet);

	private:
		f32					mEtaI;
		Spectrum			mEtaT;
		Spectrum			mK;




	};
}