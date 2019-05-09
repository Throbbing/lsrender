#include<material/matte.h>

#include<texture/texture.h>
#include<texture/constantTexture.h>
ls::Spectrum ls::Matte::refectance(ls_Param_In const IntersectionRecord& ir)
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
