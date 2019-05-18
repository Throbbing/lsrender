#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
namespace ls
{

	class Material:public Module
	{
		friend Scene;
		friend ResourceManager;
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