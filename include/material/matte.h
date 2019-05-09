#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<material/material.h>
#include<scatter/lambertian.h>

namespace ls
{
	class Matte :public Material
	{
	public:
		Matte()
		{
			mLambertian = NewPtr<Lambertian>();
		}

		virtual ~Matte()
		{
			ReleaselsPtr(mLambertian);
		}

		virtual ScatteringFunctionPtr	getSurfaceScattering()	{ return mLambertian; }
		virtual ScatteringFunctionPtr	getMediumScattering()	{ return nullptr; }

		virtual Spectrum refectance(
			ls_Param_In const IntersectionRecord& ir) override;
		virtual Spectrum transmittance(
			ls_Param_In const IntersectionRecord& ir) override;

		void applyReflectance(TexturePtr reflectance) { mReflectance = reflectance;}

		virtual void commit() override;


	protected:
		TexturePtr						mReflectance;
		ScatteringFunctionPtr			mLambertian;
	};
}
