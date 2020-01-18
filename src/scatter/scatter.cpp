#include<scatter/scatter.h>
#include<function/log.h>
void ls::DummyScatter::sample(ls_Param_In Sampler * sampler, ls_Param_In ls_Param_Out ScatteringRecord * rec)
{
	Unimplement;
}

f32 ls::DummyScatter::pdf(ls_Param_In const Vec3 & wi, ls_Param_In const Vec3 & wo)
{
	return 1.f;
}

ls::Spectrum ls::DummyScatter::f(ls_Param_In const Vec3 & wi, ls_Param_In const Vec3 & wo)
{
	return ls::Spectrum(1.f);
}
