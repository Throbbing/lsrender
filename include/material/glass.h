#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<material/material.h>
#include<scatter/dielectric.h>

namespace ls
{
	class Glass :public Material
	{
		friend ResourceManager;
	public:
		Glass()
		{
		
		}

		virtual ~Glass()
		{
			
		}

		virtual ScatteringFunctionPtr	getSurfaceScattering() { return mDielectric; }
		virtual ScatteringFunctionPtr	getMediumScattering() { return nullptr; }

		virtual Spectrum reflectance(
			ls_Param_In const IntersectionRecord& ir) override;
		virtual Spectrum transmittance(
			ls_Param_In const IntersectionRecord& ir) override;

		void applyReflectance(TexturePtr reflectance) { mReflectance = reflectance; }
		void applyTransmittance(TexturePtr transmittance) { mTransmittance = transmittance; }

		virtual void commit() override;
		virtual std::string strOut() const override;

	protected:
		Glass(ParamSet& paramSet);

	protected:
		f32								mEtaI;
		f32								mEtaT;
		TexturePtr						mReflectance;
		TexturePtr						mTransmittance;
		ScatteringFunctionPtr			mDielectric;
	};
}
