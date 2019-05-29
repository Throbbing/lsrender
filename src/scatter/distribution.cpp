#include<scatter/distribution.h>
#include<spectrum/spectrum.h>

ls::Spectrum ls::Distribution::D(ls_Param_In const Vec3 & wi, ls_Param_In const Vec3 & wo) const
{
	return D(normalize(wi + wo));
}

f32 ls::Distribution::pdf(ls_Param_In const Vec3 & wo, ls_Param_In const Vec3 & wi) const
{
	return pdf(normalize(wi + wo));
}
