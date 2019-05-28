#include<material/specularMetal.h>
#include<scatter/conductor.h>
#include<texture/texture.h>
#include<resource/resourceManager.h>
#include<resource/xmlHelper.h>

ls::Spectrum ls::SpecularMetal::reflectance(ls_Param_In const IntersectionRecord & ir)
{
	return mReflectance->fetch(&ir);
}

ls::Spectrum ls::SpecularMetal::transmittance(ls_Param_In const IntersectionRecord & ir)
{
	return Spectrum();
}

void ls::SpecularMetal::commit()
{
}

std::string ls::SpecularMetal::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Material: " << "SpecularMetal" << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::SpecularMetal::SpecularMetal(ParamSet & paramSet)
{
	auto reflectanceColor = paramSet.querySpectrum("reflectance");
	
	mEtaI = paramSet.queryf32("etaI", 1.f);
	mEtaT = paramSet.querySpectrum("etaT", 1.5f);
	mK = paramSet.querySpectrum("k", 1.f);

	auto reflectanceParamSet = ParamSet("texture", "constantTexture", "", " ");
	reflectanceParamSet.addSpectrum("color", reflectanceColor);
	mReflectance = ResourceManager::createTexture(reflectanceParamSet);

	ParamSet conductorParamSet = ParamSet("scatteringFunction", "conductor", "", "");
	conductorParamSet.addf32("etaI", mEtaI);
	conductorParamSet.addSpectrum("etaT", mEtaT);
	conductorParamSet.addSpectrum("k", mK);
	mConductor = ResourceManager::createScatteringFunction(conductorParamSet);

	

	
	
}
