#pragma once
#include<scatter/scatter.h>
#include<config/declaration.h>
#include<config/lsPtr.h>

namespace ls
{

	class Dielectric :public ScatteringFunction
	{
		friend ResourceManager;
	public:
		Dielectric(f32 etaI,f32 etaT) :
			ScatteringFunction(ScatteringFlag::EScattering_S
				| ScatteringFlag::EScattering_Reflection
				| ScatteringFlag::EScattering_Transmission
				| ScatteringFlag::EScattering_Surface) 
		{
			mEtaI = etaI;
			mEtaT = etaT;
		}


		virtual bool isDelta() { return true; }

		virtual void sample(ls_Param_In Sampler* sampler,
			ls_Param_Out ScatteringRecord* rec) override;

		virtual f32 pdf(ls_Param_In const Vec3& wo) override;
		virtual ls::Spectrum f(ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wo);

		virtual void commit() override {};
		virtual std::string strOut() const override;

	protected:
		Dielectric(ParamSet& paramSet);

	private:
		f32			mEtaI;
		f32			mEtaT;




	};
}