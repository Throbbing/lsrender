#include<material/glossyMetal.h>
#include<scatter/microfacetConductor.h>
#include<texture/texture.h>
#include<resource/resourceManager.h>
#include<resource/xmlHelper.h>

ls::Spectrum ls::GlossyMetal::reflectance(ls_Param_In const IntersectionRecord & ir)
{
	return mReflectance->fetch(&ir);
}

ls::Spectrum ls::GlossyMetal::transmittance(ls_Param_In const IntersectionRecord & ir)
{
	return 0.f;
}

std::string ls::GlossyMetal::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Material: " << "SpecularMetal" << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::GlossyMetal::GlossyMetal(ParamSet & paramSet)
{
	auto reflectanceColor = paramSet.querySpectrum("reflectance");

	mEtaI = paramSet.queryf32("etaI", 1.f);
	mEtaT = paramSet.querySpectrum("etaT", 1.5f);
	mK = paramSet.querySpectrum("k", 1.f);
	mAlphaU = paramSet.queryf32("alphaU", 1.f);
	mAlphaV = paramSet.queryf32("alphaV", 1.f);

	auto reflectanceParamSet = ParamSet("texture", "constantTexture", "", " ");
	reflectanceParamSet.addSpectrum("color", reflectanceColor);
	mReflectance = ResourceManager::createTexture(reflectanceParamSet);

	auto type = paramSet.queryString("distribution", "ggx");

	ParamSet conductorParamSet = ParamSet("scatteringFunction", "microfacetConductor", "", "");
	conductorParamSet.addf32("etaI", mEtaI);
	conductorParamSet.addSpectrum("etaT", mEtaT);
	conductorParamSet.addSpectrum("k", mK);
	conductorParamSet.addf32("alphaU", mAlphaU);
	conductorParamSet.addf32("alphaV", mAlphaV);
	conductorParamSet.addbool("sampleAll", paramSet.querybool("sampleAll", true));
	conductorParamSet.addString("distribution", type);

	mMicrofacetConductor = ResourceManager::createScatteringFunction(conductorParamSet);
}
