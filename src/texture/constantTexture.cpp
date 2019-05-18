#include<texture/constantTexture.h>
#include<resource/xmlHelper.h>
void ls::ConstantTexture::commit()
{
}

std::string ls::ConstantTexture::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Texture: " << "Constant" << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::Spectrum ls::ConstantTexture::fetch(ls_Param_In const IntersectionRecord * rec)
{
	return mColor;
}

ls::ConstantTexture::ConstantTexture(ParamSet & paramSet):Texture(ETexConstant)
{
	mColor = paramSet.querySpectrum("color");
}
