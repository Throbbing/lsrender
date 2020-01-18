#include<material/material.h>
#include<record/record.h>
#include<spectrum/spectrum.h>
#include<scatter/scatter.h>
ls::Spectrum ls::Material::scatteringFactor(ls_Param_In const ScatteringRecord & sr, ls_Param_In const IntersectionRecord & ir)
{
	if (sr.scatterFlag & EScattering_Reflection)
		return reflectance(ir);
	else
		return transmittance(ir);
}

ls::DummyMaterial::DummyMaterial()
{
	mDummyScatter = new DummyScatter();
}

ls::DummyMaterial::~DummyMaterial()
{
	delete mDummyScatter;
}

ls::Spectrum ls::DummyMaterial::reflectance(ls_Param_In const IntersectionRecord & ir)
{
	return Spectrum(1.f);
}

ls::Spectrum ls::DummyMaterial::transmittance(ls_Param_In const IntersectionRecord & ir)
{
	return Spectrum(1.f);
}
