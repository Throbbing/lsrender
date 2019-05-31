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

	Ray ray = Ray(p0, dir, 0, time, 1e-4f, dist * (1.f - 1e-3f));

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

void ls::RenderLib::fillScatteringRecordForBSDFValueAndPdf(
	ls_Param_In const Point3 & pos, 
	ls_Param_In const Normal & normal, 
	ls_Param_In const Vec3 & wo, 
	ls_Param_In const Vec3 & wi, 
	ls_Param_In u32 mode, 
	ls_Param_In ls_Param_Out ScatteringRecord * sr)
{
	sr->position = pos;
	sr->normal = normal;
	sr->wi = wi;
	sr->wo = wo;
	sr->transportMode = mode;

}

void ls::RenderLib::fillScatteringRecordForBSDFSample(
	ls_Param_In const Point3 & pos, 
	ls_Param_In const Normal & normal, 
	ls_Param_In const Vec3 & wo, 
	ls_Param_In u32 mode, 
	ls_Param_In ls_Param_Out ScatteringRecord * sr)
{
	sr->position = pos;
	sr->normal = normal;
	sr->wo = wo;
	sr->transportMode = mode;
}



void ls::RenderLib::surfaceBSDFValueAndPdf(
	ls_Param_In ScatteringFunctionPtr bsdf, 
	ls_Param_In ls_Param_Out ScatteringRecord* sr)
{
	Frame frame(sr->normal);
	auto wi = frame.toLocal(sr->wi);
	auto wo = frame.toLocal(sr->wo);

	sr->sampledValue = bsdf->f(wi, wo);
	sr->pdf = bsdf->pdf(wi,wo);

	//Í¬°ëÇò£¬·´Éä
	if (Frame::hemisphere(wi, wo))
	{
		sr->scatterFlag = (bsdf->scatteringFlag() & (~EScattering_Transmission));
	}
	//Òì°ëÇò£¬ÕÛÉä
	else
	{
		sr->scatterFlag = (bsdf->scatteringFlag() & (~EScattering_Reflection));
	}

}

void ls::RenderLib::sampleSurfaceBSDF(
	ls_Param_In SamplerPtr sampler,
	ls_Param_In ScatteringFunctionPtr bsdf,
	ls_Param_In ls_Param_Out ScatteringRecord * sr)
{
	Frame frame(sr->normal);
	auto localWo = normalize(frame.toLocal(sr->wo));
	auto globalWo = sr->wo;
	sr->wo = localWo;

	bsdf->sample(sampler, sr);
	sr->wi = normalize(frame.toWorld(sr->wi));
	sr->wo = globalWo;
	
}

f32 ls::RenderLib::snellLaw(ls_Param_In f32 etaI, ls_Param_In f32 cosThetaI, ls_Param_In f32 etaT)
{
	f32 sinThetaI = std::sqrtf(std::max(0.f, 1.f - cosThetaI * cosThetaI));

	f32 sinThetaT = etaI / etaT * sinThetaI;

	f32 cosThetaT = std::sqrtf(std::max(0.f, 1.f - sinThetaT * sinThetaT));
	return cosThetaT;
}

f32 ls::RenderLib::fresnelDielectric(f32 cosThetaI, f32 etaI, f32 cosThetaT, f32 etaT)
{
	f32 temp1 = ((etaT * cosThetaI) - (etaI * cosThetaT)) /
		((etaT * cosThetaI) + (etaI * cosThetaT));
	f32 temp2 = ((etaI * cosThetaI) - (etaT* cosThetaT)) /
		((etaI * cosThetaI) + (etaT*cosThetaT));

	return (temp1* temp1 + temp2 *temp2) *0.5f;
}

f32 ls::RenderLib::fresnelConductor(f32 cosThetaI, f32 etaI, f32 etaT, f32 k)
{
	//From PBRT-V3
	f32 eta = etaT / etaI;
	f32 etak = k / etaI;

	f32 cosThetaI2 = cosThetaI * cosThetaI;
	f32 sinThetaI2 = 1. - cosThetaI2;
	f32 eta2 = eta * eta;
	f32 etak2 = etak * etak;

	f32 t0 = eta2 - etak2 - sinThetaI2;
	f32 a2plusb2 = std::sqrtf(t0 * t0 + 4 * eta2 * etak2);
	f32 t1 = a2plusb2 + cosThetaI2;
	f32 a = std::sqrtf(0.5f * (a2plusb2 + t0));
	f32 t2 = (f32)2 * cosThetaI * a;
	f32 Rs = (t1 - t2) / (t1 + t2);

	f32 t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
	f32 t4 = t2 * sinThetaI2;
	f32 Rp = Rs * (t3 - t4) / (t3 + t4);

	return 0.5 * (Rp + Rs);
}

ls::Spectrum ls::RenderLib::fresnelConductor(f32 cosThetaI, f32 etaI, Spectrum etaT, Spectrum k)
{
	//From PBRT-V3
	auto eta = etaT / etaI;
	auto etak = k / etaI;

	f32 cosThetaI2 = cosThetaI * cosThetaI;
	f32 sinThetaI2 = 1. - cosThetaI2;
	auto eta2 = eta * eta;
	auto etak2 = etak * etak;

	auto t0 = eta2 - etak2 - sinThetaI2;
	auto a2plusb2 = sqrt(t0 * t0 + 4 * eta2 * etak2);
	auto t1 = a2plusb2 + cosThetaI2;
	auto a = sqrt(0.5f * (a2plusb2 + t0));
	auto t2 = (f32)2 * cosThetaI * a;
	auto Rs = (t1 - t2) / (t1 + t2);

	auto t3 = cosThetaI2 * a2plusb2 + sinThetaI2 * sinThetaI2;
	auto t4 = t2 * sinThetaI2;
	auto Rp = Rs * (t3 - t4) / (t3 + t4);

	return 0.5f * (Rp + Rs);
}

f32 ls::RenderLib::beckmanG1(const Vec3 & v, const Vec3 & m, f32 alpha)
{
	//From mitsuba0.5.0
	if (dot(v, m) * Frame::cosTheta(v) <= 0)
		return 0.0f;

	f32 tanTheta = std::abs(Frame::tanTheta(v));
	if (tanTheta == 0.0f)
		return 1.0f;

	f32 a = 1.0f / (alpha * tanTheta);
	if (a >= 1.6f)
		return 1.0f;

	/* Use a fast and accurate (<0.35% rel. error) rational
	approximation to the shadowing-masking function */
	f32 aSqr = a*a;
	return (3.535f * a + 2.181f * aSqr)
		/ (1.0f + 2.276f * a + 2.577f * aSqr);
}

f32 ls::RenderLib::ggxG1(const Vec3 & v, const Vec3 & m, f32 alpha)
{
	if (dot(v, m) * Frame::cosTheta(v) <= 0)
		return 0.0f;

	f32 tanTheta = std::abs(Frame::tanTheta(v));
	if (tanTheta == 0.0f)
		return 1.0f;

	f32 a = 1.0f / (alpha * tanTheta);
	if (a >= 1.6f)
		return 1.0f;

	f32 root = alpha * tanTheta;
	f32 temp = 1 + root *root;
	if (temp < 0.f) return 0.f;

	return 2.0f / (1.0f + std::sqrtf(temp));
}

f32 ls::RenderLib::beckmanDistribution(const Vec3& wh,
	f32 alphaU,
	f32 alphaV)
{
	auto tanTheta2 = Frame::tanTheta2(wh);
	if (std::isnan(tanTheta2)) return 0.f;
	
	f32 cosTheta2 = Frame::cosTheta(wh) *Frame::cosTheta(wh);
	f32 cosTheta4 = cosTheta2 * cosTheta2;
	f32 cosPhi = Frame::cosPhi(wh);
	f32 sinPhi = Frame::sinPhi(wh);
	f32 cosPhi2 = cosPhi * cosPhi;
	f32 sinPhi2 = sinPhi * sinPhi;

	return std::expf(-tanTheta2 * (cosPhi2 / (alphaU * alphaU) +
		sinPhi2 / (alphaV * alphaV))) /
		(lsMath::PI * alphaU * alphaV * cosTheta4);


}

f32 ls::RenderLib::ggxDistribution(const Vec3& wh,
	f32 alphaU,
	f32 alphaV)
{
	auto tanTheta2 = Frame::tanTheta2(wh);
	if (std::isnan(tanTheta2)) return 0.f;

	f32 cosTheta2 = Frame::cosTheta(wh) *Frame::cosTheta(wh);
	f32 cosTheta4 = cosTheta2 * cosTheta2;
	f32 cosPhi = Frame::cosPhi(wh);
	f32 sinPhi = Frame::sinPhi(wh);
	f32 cosPhi2 = cosPhi * cosPhi;
	f32 sinPhi2 = sinPhi * sinPhi;

	f32 e =
		(cosPhi2 / (alphaU * alphaV) + sinPhi2 / (alphaU * alphaV)) *
		tanTheta2;
	return 1 / (lsMath::PI * alphaU * alphaV * cosTheta4 * (1 + e) * (1 + e));
}

f32 ls::MonteCarlo::beckmanDistributionAllPdf(const Vec3 & w, const Vec3 & wh, f32 alphaU, f32 alphaV)
{
	return RenderLib::beckmanDistribution(wh, alphaU, alphaV) * Frame::absCosTheta(wh);
}

f32 ls::MonteCarlo::beckmanDistributionVisiblePdf(const Vec3 & w, const Vec3 & wh, f32 alphaU, f32 alphaV)
{
	if (lsMath::closeZero(Frame::cosTheta(w)))
		return 0.f;

	auto sinPhi = Frame::sinPhi(w);
	auto sinPhi2 = sinPhi * sinPhi;
	auto cosPhi = Frame::cosPhi(w);
	auto cosPhi2 = cosPhi* cosPhi;

	auto alpha = std::sqrt(cosPhi2 * alphaU * alphaU + sinPhi2 * alphaV * alphaV);

	return RenderLib::beckmanDistribution(wh, alphaU, alphaV) * RenderLib::beckmanG1(w, wh, alpha) * std::fabs(dot(w, wh)) / Frame::absCosTheta(w);
}

f32 ls::MonteCarlo::ggxDistributionAllPdf(const Vec3 & w, const Vec3 & wh, f32 alphaU, f32 alphaV)
{
	return RenderLib::ggxDistribution(wh, alphaU, alphaV) * Frame::absCosTheta(wh);
}

f32 ls::MonteCarlo::ggxDistributionVisiblePdf(const Vec3 & w, const Vec3 & wh, f32 alphaU, f32 alphaV)
{
	if (lsMath::closeZero(Frame::cosTheta(w)))
		return 0.f;

	auto sinPhi = Frame::sinPhi(w);
	auto sinPhi2 = sinPhi * sinPhi;
	auto cosPhi = Frame::cosPhi(w);
	auto cosPhi2 = cosPhi* cosPhi;

	auto alpha = std::sqrt(cosPhi2 * alphaU * alphaU + sinPhi2 * alphaV * alphaV);
	return RenderLib::ggxDistribution(wh, alphaU, alphaV) * RenderLib::ggxG1(w, wh, alpha) *std::fabs(dot(w, wh)) / Frame::absCosTheta(w);
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

	Vec2 uOffset = Vec2(2.f * uv.x, 2.f * uv.y)- Vec2(1, 1);
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

void ls::MonteCarlo::sampleBeckmanDistributionAll(ls_Param_In Point2 uv, 
	ls_Param_In f32 alphaU,
	ls_Param_In f32 alphaV, 
	ls_Param_Out Vec3 * wh, 
	ls_Param_Out f32 * pdf)
{
	//From PBRT-V3
	// Sample full distribution of normals for Beckmann distribution

	// Compute $\tan^2 \theta$ and $\phi$ for Beckmann distribution sample
	f32 tan2Theta, phi;
	if (alphaU == alphaV) {
		if (uv.x == 1.f) uv.x = lsMath::Epsilon;

		f32 logSample = std::log(1 - uv.x);
		tan2Theta = -alphaU * alphaV * logSample;
		phi = uv.y * 2 * lsMath::PI;
	}
	else {
		// Compute _tan2Theta_ and _phi_ for anisotropic Beckmann
		// distribution
		if (uv.x == 1.f) uv.x = lsMath::Epsilon;

		f32 logSample = std::log(1 - uv.x);
		phi = std::atanf(alphaV / alphaU *
			std::tanf(2 * lsMath::PI * uv.y + 0.5f * lsMath::PI));
		if (uv.y > 0.5f) phi += lsMath::PI;
		f32 sinPhi = std::sinf(phi), cosPhi = std::cosf(phi);
		f32 alphax2 = alphaU * alphaU, alphay2 = alphaV * alphaV;
		tan2Theta = -logSample /
			(cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
	}

	// Map sampled Beckmann angles to normal direction _wh_
	f32 cosTheta = 1 / std::sqrt(1 + tan2Theta);
	f32 sinTheta = std::sqrt(std::max((f32)0, 1 - cosTheta * cosTheta));
	
	*wh = Vec3(sinTheta * std::cosf(phi),
		sinTheta * std::sinf(phi),
		cosTheta);

	if (pdf)
		*pdf = RenderLib::beckmanDistribution(*wh, alphaU, alphaV) * Frame::absCosTheta(*wh);
	
}

void ls::MonteCarlo::sampleBeckmanDistributionVisible(ls_Param_In Point2 uv,
	ls_Param_In f32 alphaU, 
	ls_Param_In f32 alphaV, 
	ls_Param_In const Vec3& w,
	ls_Param_Out Vec3 * wh, 
	ls_Param_Out f32 * pdf)
{
	//From PBRT-V3
	bool flip = w.z < 0.f;
	Unimplement;
}

void ls::MonteCarlo::sampleGGXDistributionAll(ls_Param_In Point2 uv, 
	ls_Param_In f32 alphaU, 
	ls_Param_In f32 alphaV, 
	ls_Param_Out Vec3 * wh,
	ls_Param_Out f32 * pdf)
{
	//From PBRT-V3
	f32 cosTheta = 0, phi = (2 * lsMath::PI) * uv.y;
	if (uv.x == 1.f) uv.x = 1.f - lsMath::Epsilon;

	if (alphaU == alphaV) {
		f32 tanTheta2 = alphaU * alphaU * uv.x / (1.0f - uv.x);
		cosTheta = 1 / std::sqrt(1 + tanTheta2);
	}
	else {
		phi =
			std::atan(alphaV / alphaU * std::tan(2 * lsMath::PI * uv.y + .5f * lsMath::PI));
		if (uv.y > .5f) phi += lsMath::PI;
		f32 sinPhi = std::sin(phi), cosPhi = std::cos(phi);
		const f32 alphax2 = alphaU * alphaU, alphay2 = alphaV * alphaV;
		const f32 alpha2 =
			1 / (cosPhi * cosPhi / alphax2 + sinPhi * sinPhi / alphay2);
		f32 tanTheta2 = alpha2 * uv.x / (1 - uv.x);
		cosTheta = 1 / std::sqrt(1 + tanTheta2);
	}
	f32 sinTheta =
		std::sqrt(std::max((f32)0., (f32)1. - cosTheta * cosTheta));

	*wh = Vec3(sinTheta * std::cosf(phi),
		sinTheta * std::sinf(phi),
		cosTheta);

	if(pdf)
		*pdf = RenderLib::ggxDistribution(*wh,alphaU,alphaV) * Frame::absCosTheta(*wh);


}

void ls::MonteCarlo::sampleGGXDistributionVisible(ls_Param_In Point2 uv,
	ls_Param_In f32 alphaU, 
	ls_Param_In f32 alphaV, 
	ls_Param_In const Vec3& w,
	ls_Param_Out Vec3 * wh, 
	ls_Param_Out f32 * pdf)
{
	Unimplement;
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

ls::Distribution2D::Distribution2D(const f32 * func, int nu, int nv)
{
	pConditionalV.reserve(nv);
	for (int v = 0; v < nv; ++v) {
		// Compute conditional sampling distribution for $\tilde{v}$
		pConditionalV.emplace_back(new Distribution1D(&func[v * nu], nu));
	}
	// Compute marginal sampling distribution $p[\tilde{v}]$
	std::vector<f32> marginalFunc;
	marginalFunc.reserve(nv);
	for (int v = 0; v < nv; ++v)
		marginalFunc.push_back(pConditionalV[v]->funcInt);
	pMarginal.reset(new Distribution1D(&marginalFunc[0], nv));
}
