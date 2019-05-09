#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>

namespace ls
{

	class Material
	{
		friend Scene;
	public:
		Material() {}
		virtual ~Material() {}

		virtual ScatteringFunctionPtr getSurfaceScattering() { return nullptr; }
		virtual ScatteringFunctionPtr getMediumScattering()  { return nullptr; }

		virtual Spectrum refectance(
			ls_Param_In const IntersectionRecord& ir) = 0;
		virtual Spectrum transmittance(
			ls_Param_In const IntersectionRecord& ir) = 0;

		virtual void commit() = 0;
	};
}