#include<function/func.h>
#include<math/math.h>

f32 ls::RenderLib::mis(s32 nf, f32 pf, s32 ng, f32 pg)
{
	auto a = nf*pf;
	auto b = ng*pg;

	return lsMath::Square(a) / (lsMath::Square(a) + lsMath::Square(b));
}

f32 ls::RenderLib::mis(f32 pf, f32 pg)
{
	return (pf*pf) / (pf*pf + pg*pg);
}

f32 ls::RenderLib::pdfW2A(f32 pdfW, f32 r, f32 cos)
{
	return pdfW*cos / (r*r);
}

f32 ls::RenderLib::pdfA2W(f32 pdfA, f32 r, f32 cos)
{
	return pdfA*r*r / cos;
}

bool ls::RenderLib::visible(Scene * scene, Point p0, Point p1)
{
	Unimplement;
	return false;
}

bool ls::RenderLib::globalSample(ls_Param_In Scene * scene, ls_Param_In Sampler * sampler, ls_Param_Out MeshSampleRecord * meshRec)
{
	Unimplement;
	return false;
}

