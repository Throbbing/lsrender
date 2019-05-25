#include<material/matte.h>
#include<resource/resourceManager.h>
#include<resource/xmlHelper.h>
#include<texture/texture.h>
#include<texture/constantTexture.h>
ls::Spectrum ls::Matte::reflectance(ls_Param_In const IntersectionRecord& ir)
{
	return mReflectance->fetch(&ir);
}

ls::Spectrum ls::Matte::transmittance(ls_Param_In const IntersectionRecord& ir)
{
	return Spectrum(0.f);
}

void ls::Matte::commit()
{
}

std::string ls::Matte::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Material: " << "Matte" << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::Matte::Matte(ParamSet & paramSet)
{
	auto tex = paramSet.queryParamSetByName("reflectance");

	mReflectance = ResourceManager::createTexture(tex);

	ParamSet lambertian = ParamSet("scatteringFunction", "lambertian", "lambertian", "");
	mLambertian = ResourceManager::createScatteringFunction(lambertian);
}
