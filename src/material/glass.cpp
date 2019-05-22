#include<material/glass.h>
#include<scatter/dielectric.h>
#include<texture/texture.h>
#include<resource/resourceManager.h>
#include<resource/xmlHelper.h>
ls::Spectrum ls::Glass::reflectance(ls_Param_In const IntersectionRecord & ir)
{
	return mReflectance->fetch(&ir);
}

ls::Spectrum ls::Glass::transmittance(ls_Param_In const IntersectionRecord & ir)
{
	return mTransmittance->fetch(&ir);
}

void ls::Glass::commit()
{
}

std::string ls::Glass::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Material: " << "Glass" << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::Glass::Glass(ParamSet & paramSet)
{
	auto reflectanceColor = paramSet.querySpectrum("reflectance");
	auto transmittanceColor = paramSet.querySpectrum("transmittance");
	f32 mEtaI = paramSet.queryf32("etaI");
	f32 mEtaT = paramSet.queryf32("etaT");

	ParamSet reflectParamSet = ParamSet("texture", "constantTexture", "", "");
	reflectParamSet.addSpectrum("color", reflectanceColor);

	ParamSet transParamSet = ParamSet("texture", "constantTexture", "", "");
	transParamSet.addSpectrum("color", transmittanceColor);

	mReflectance = ResourceManager::createTexture(reflectParamSet);
	mTransmittance = ResourceManager::createTexture(transParamSet);

	ParamSet dielectricParamSet = ParamSet("scatteringFunction", "dielectric", "", "");
	dielectricParamSet.addf32("etaI", mEtaI);
	dielectricParamSet.addf32("etaT", mEtaT);
	
	mDielectric = ResourceManager::createScatteringFunction(dielectricParamSet);
}
