#include<texture/constantTexture.h>

void ls::ConstantTexture::commit()
{
}

ls::Spectrum ls::ConstantTexture::fetch(ls_Param_In const IntersectionRecord * rec)
{
	return mColor;
}
