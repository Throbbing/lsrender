#include<function\func.h>
#include<function\stru.h>
#include<math/header.h>
#include<record/record.h>

#include<scatter/scatter.h>
#include<scene/scene.h>

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

bool ls::RenderLib::visible(ScenePtr scene, Point p0, Point p1,f32 time)
{
	auto dir = Vec3(p1 - p0);
	auto dist = dir.length();
	dir /= dist;

	Ray ray = Ray(p0, dir, 0, time, 1.f, dist - 0.1f);

	return !scene->occlude(ray);
}

bool ls::RenderLib::globalSample(
	ls_Param_In ScenePtr scene,
	ls_Param_In SamplerPtr sampler, 
	ls_Param_Out MeshSampleRecord * meshRec)
{
	Unimplement;
	return false;
}

bool ls::RenderLib::matchScatterFlag(ls_Param_In ScatteringFunctionPtr scatter, ls_Param_In u32 flag)
{
	return (scatter->scatteringFlag() & flag);
}



void ls::RenderLib::surfaceBSDFValueAndPdf(
	ls_Param_In ScatteringFunctionPtr bsdf, 
	ls_Param_In ls_Param_Out ScatteringRecord* sr)
{
	Frame frame(sr->normal);
	auto wi = frame.toLocal(sr->wi);
	auto wo = frame.toLocal(sr->wo);
	sr->sampleValue = bsdf->f(wi, wo);
	sr->pdf = bsdf->pdf(wi);
}

void ls::RenderLib::sampleSurfaceBSDF(
	ls_Param_In SamplerPtr sampler,
	ls_Param_In ScatteringFunctionPtr bsdf,
	ls_Param_In ls_Param_Out ScatteringRecord * sr)
{
	Frame frame(sr->normal);
	auto localWo = sr->wo;
	bsdf->sample(sampler, sr);
	sr->sampleValue = bsdf->f(sr->wi, localWo);
	sr->wi = frame.toWorld(sr->wi);
	
	
}

void ls::MonteCarlo::sampleHemisphere(ls_Param_In Point2 uv, ls_Param_Out Vec3 * w, ls_Param_Out f32 * pdf)
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

void ls::MonteCarlo::sampleCosHemisphere(ls_Param_In Point2 uv, ls_Param_Out Vec3 * w, ls_Param_Out f32 * pdf)
{
	Point2 d;
	f32 pdfT;
	sampleConcentricDisk(uv, &d, &pdfT);

	f32 z = std::sqrt(std::max((f32)0, 1 - d.x * d.x - d.y * d.y));
	*w = Vec3(d.x, d.y, z);
	*pdf = sampleCosHemispherePdf(*w);
}

f32 ls::MonteCarlo::sampleCosHemispherePdf(const Vec3 &w)
{
	return std::fabs(w.z) * lsMath::InvPi;
}

void ls::MonteCarlo::sampleSphere(ls_Param_In Point2 uv, ls_Param_Out Vec3 * w, ls_Param_Out f32 * pdf)
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

void ls::MonteCarlo::sampleDisk(ls_Param_In Point2 uv, ls_Param_Out Point2 * p, ls_Param_Out f32 * pdf)
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

void ls::MonteCarlo::sampleConcentricDisk(ls_Param_In Point2 uv, ls_Param_Out Point2 * p, ls_Param_Out f32 * pdf)
{

	Vec2 uOffset = Vec2(2.f * uv.x, 2.f * uv.x)- Vec2(1, 1);
	// Handle degeneracy at the origin
	if (uOffset.x == 0 && uOffset.y == 0) 
	{
		*p = Point2(0.f);
		*pdf = 0.f;
		return;
	}

	// Apply concentric mapping to point
	f32 theta, r;
	if (std::abs(uOffset.x) > std::abs(uOffset.y)) {
		r = uOffset.x;
		theta = lsMath::PI * 0.25f * (uOffset.y / uOffset.x);
	}
	else {
		r = uOffset.y;
		theta = lsMath::PI * 0.5f  - lsMath::PI * 0.25f * (uOffset.x / uOffset.y);
	}
	*p =  Point2(r * Vec2(std::cos(theta), std::sin(theta)));
	*pdf = sampleConcentricDistPdf(*p);
}

f32 ls::MonteCarlo::sampleConcentricDistPdf(const Point2 & p)
{
	return 1.f;
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
	rtcRay.flags = 0;
	return rtcRay;
}
