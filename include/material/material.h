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
		Material(const std::string& id = "material"):Module(id){}
		virtual ~Material() {}

		virtual ScatteringFunctionPtr getSurfaceScattering() { return nullptr; }
		virtual ScatteringFunctionPtr getMediumScattering()  { return nullptr; }

		virtual Spectrum reflectance(
			ls_Param_In const IntersectionRecord& ir) = 0;
		virtual Spectrum transmittance(
			ls_Param_In const IntersectionRecord& ir) = 0;

		virtual Spectrum scatteringFactor(
			ls_Param_In const ScatteringRecord& sr,
			ls_Param_In const IntersectionRecord& ir);

		virtual void commit() = 0;
	};

	class DummyMaterial :public Material
	{
	public:
		DummyMaterial();
		virtual ~DummyMaterial();

		virtual ScatteringFunctionPtr getSurfaceScattering() { return mDummyScatter; }
		virtual ScatteringFunctionPtr getMediumScattering() { return mDummyScatter; }

		virtual Spectrum reflectance(ls_Param_In const IntersectionRecord& ir);
		virtual Spectrum transmittance(ls_Param_In const IntersectionRecord& ir);

		virtual std::string strOut() const override {
			return "DummyMaterial";
		}
		virtual void commit() override {
			
		}

	protected:
		ScatteringFunctionPtr mDummyScatter;

	};
}