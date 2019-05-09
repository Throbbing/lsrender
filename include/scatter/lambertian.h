#pragma once
#include<scatter/scatter.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
namespace ls
{
	
	class Lambertian :public ScatteringFunction
	{
	public:
		Lambertian() :
			ScatteringFunction(ScatteringFlag::EScattering_D
				| ScatteringFlag::EScattering_Surface) {}


		virtual bool isDelta() { return false; }

		virtual void sample(ls_Param_In Sampler* sampler,
			ls_Param_Out ScatteringRecord* rec) override;

		virtual f32 pdf(ls_Param_In const Vec3& wo) override;
		virtual ls::Spectrum f(ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wo);

	private:

		



	};
}