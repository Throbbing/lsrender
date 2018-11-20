#include<function\func.h>
#include<function\stru.h>
#include<math\math.h>

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

void ls::MonteCarlo::sampleHemisphere(ls_Param_In Vec2 uv, ls_Param_Out Vec3 * w, ls_Param_Out f32 * pdf)
{
	auto z = uv.x;
	f32 r = std::sqrt(std::max((f32)0, (float)1.f - z * z));
	f32 phi = 2 * lsMath::PI * uv.y;
	*w = Vec3(r * std::cos(phi), r * std::sin(phi), z);
	*pdf = sampleHemispherePdf(*w);
}

f32 ls::MonteCarlo::sampleHemispherePdf(const Vec3 & w)
{
	return lsMath::Inv2Pi;
}

void ls::MonteCarlo::sampleCosHemisphere(ls_Param_In Vec2 uv, ls_Param_Out Vec3 * w, ls_Param_Out f32 * pdf)
{
	Unimplement;
}

f32 ls::MonteCarlo::sampleCosHemisphere(const Vec3 & w)
{
	Unimplement;
	return f32();
}

void ls::MonteCarlo::sampleSphere(ls_Param_In Vec2 uv, ls_Param_Out Vec3 * w, ls_Param_Out f32 * pdf)
{
	f32 z = 1 - 2 * uv[0];
	f32 r = std::sqrt(std::max((f32)0, (f32)1 - z * z));
	f32 phi = 2 * lsMath::PI * uv[1];
	*w =  Vec3(r * std::cos(phi), r * std::sin(phi), z);
	*pdf = sampleSpherePdf(*w);
}

f32 ls::MonteCarlo::sampleSpherePdf(const Vec3 & w)
{
	return lsMath::Inv4Pi;
}

void ls::MonteCarlo::sampleDisk(ls_Param_In Vec2 uv, ls_Param_Out Point2 * p, ls_Param_Out f32 * pdf)
{
	f32 r = std::sqrt(uv[0]);
	f32 theta = 2 * lsMath::PI * uv[1];
	*p =  Point2(r * std::cos(theta), r * std::sin(theta));
	*pdf = sampleDistPdf(*p);
}

f32 ls::MonteCarlo::sampleDistPdf(const Point2 & p)
{
	return lsMath::Inv2Pi;
}

void ls::MonteCarlo::sampleConcentricDisk(ls_Param_In Vec2 uv, ls_Param_Out Point2 * p, ls_Param_Out f32 * pdf)
{
	Unimplement;
}

f32 ls::MonteCarlo::sampleConcentricDistPdf(const Point2 & p)
{
	Unimplement;
	return f32();
}

RTCRay ls::GeometryLib::lsRay2Embree(const Ray & ray)
{
	RTCRay rtcRay;
	rtcRay.org_x = ray.ori.x;
	rtcRay.org_y = ray.ori.y;
	rtcRay.org_z = ray.ori.z;
	rtcRay.dir_x = ray.dir.x;
	rtcRay.dir_y = ray.dir.y;
	rtcRay.dir_z = ray.dir.z;
	rtcRay.tnear = ray.tnear;
	rtcRay.tfar = ray.tfar;
	rtcRay.time = ray.time;

	return rtcRay;
}
