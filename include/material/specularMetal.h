#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<material/material.h>
#include<scatter/conductor.h>

namespace ls
{
	class SpecularMetal :public Material
	{
		friend ResourceManager;
	public:
		SpecularMetal()
		{

		}

		virtual ~SpecularMetal()
		{

		}

		virtual ScatteringFunctionPtr	getSurfaceScattering() { return mConductor; }
		virtual ScatteringFunctionPtr	getMediumScattering() { return nullptr; }

		virtual Spectrum reflectance(
			ls_Param_In const IntersectionRecord& ir) override;
		virtual Spectrum transmittance(
			ls_Param_In const IntersectionRecord& ir) override;

		void applyReflectance(TexturePtr reflectance) { mReflectance = reflectance; }
		void applyTransmittance(TexturePtr transmittance) {  }

		virtual void commit() override;
		virtual std::string strOut() const override;

	protected:
		SpecularMetal(ParamSet& paramSet);

	protected:
		f32								mEtaI;
		Spectrum						mEtaT;
		Spectrum						mK;
		TexturePtr						mReflectance;
		ScatteringFunctionPtr			mConductor;
	};
}
