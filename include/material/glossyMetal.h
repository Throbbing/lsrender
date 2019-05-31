#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<material/material.h>
#include<scatter/conductor.h>

namespace ls
{
	class GlossyMetal :public Material
	{
		friend ResourceManager;
	public:
		GlossyMetal()
		{

		}

		virtual ~GlossyMetal()
		{

		}

		virtual ScatteringFunctionPtr	getSurfaceScattering() { return mMicrofacetConductor; }
		virtual ScatteringFunctionPtr	getMediumScattering() { return nullptr; }

		virtual Spectrum reflectance(
			ls_Param_In const IntersectionRecord& ir) override;
		virtual Spectrum transmittance(
			ls_Param_In const IntersectionRecord& ir) override;

		void applyReflectance(TexturePtr reflectance) { mReflectance = reflectance; }
		void applyTransmittance(TexturePtr transmittance) {  }

		virtual void commit() override {}
		virtual std::string strOut() const override;

	protected:
		GlossyMetal(ParamSet& paramSet);

	protected:
		f32								mEtaI;
		Spectrum						mEtaT;
		Spectrum						mK;
		f32								mAlphaU;
		f32								mAlphaV;
		TexturePtr						mReflectance = nullptr;
		ScatteringFunctionPtr			mMicrofacetConductor = nullptr;
	};
}
