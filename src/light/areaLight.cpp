#include<config/common.h>
#include<light/areaLight.h>
#include<math/math.h>
#include<mesh/mesh.h>
#include<resource/xmlHelper.h>
#include<scatter/scatter.h>
#include<scene/scene.h>

ls::Spectrum ls::AreaLight::getPower()
{
	return mRadiance * mMesh->getArea() * lsMath::PI;
}

void ls::AreaLight::sample(ls_Param_In SamplerPtr sampler, ls_Param_Out LightSampleRecord * rec)
{
	Unimplement;
}

void ls::AreaLight::sample(ls_Param_In SamplerPtr sampler, ls_Param_In const IntersectionRecord * refRec, ls_Param_Out LightSampleRecord * rec)
{
	MeshSampleRecord meshRefRec;
	MeshSampleRecord meshRec;
	mMesh->sample(sampler, &meshRefRec, &meshRec);

	auto dir = Vec3(refRec->position - meshRec.samplePosition);
	auto r = dir.length();

	if (r == 0.f)
	{
		rec->le = 0.f;
		rec->pdfPos = 0.f;
		return;
	}

	dir /= r;
	rec->le = dot(dir, meshRec.surfaceNormal) > 0.f ? mRadiance : 0.f;
	rec->samplePosition = meshRec.samplePosition;
	rec->sampleDirection = dir;
	rec->pdfPos = 1.f / mMesh->getArea();
	rec->pdfDir = RenderLib::pdfA2W(rec->pdfPos, r, dot(meshRec.surfaceNormal, dir));
	rec->pdfW = rec->pdfDir;
	rec->mode = EMeasure_SolidAngle;
	rec->light = LightPtr(this);

	
}

ls::Spectrum ls::AreaLight::sample(const Ray & ray)
{
	IntersectionRecord its;
	Ray r = ray;
	lsRender::scene->intersect(r, &its);
	if (!lsRender::scene->intersect(r, &its) || its.areaLight != this)
	{
		return 0.f;
	}

	return dot(its.ns, -r.dir) > 0.f ? mRadiance : 0.f;

}

ls::Spectrum ls::AreaLight::sample(const Ray & ray, const IntersectionRecord & its)
{
	return dot(its.ns, -ray.dir) > 0.f ? mRadiance : 0.f;
}

f32 ls::AreaLight::pdf(const Ray & ray)
{
	IntersectionRecord its;
	Ray r = ray;
	lsRender::scene->intersect(r, &its);
	if (!lsRender::scene->intersect(r, &its) || its.areaLight != this)
	{
		return 0.f;
	}

	auto l2p = Vector(ray.ori - its.position);
	auto dist = l2p.length();
	l2p /= dist;
	auto hitNormal = its.ns;

	return dot(its.ns, -ray.dir) > 0.f ? RenderLib::pdfA2W(1.f / mMesh->getArea(), dist, dot(its.ns, -ray.dir)) : 0.f;

}

f32 ls::AreaLight::pdf(const Ray & ray, const IntersectionRecord & its)
{
	auto l2p = Vector(ray.ori - its.position);
	auto dist = l2p.length();
	l2p /= dist;
	auto hitNormal = its.ns;

	return dot(its.ns, -ray.dir) > 0.f ? RenderLib::pdfA2W(1.f / mMesh->getArea(), dist, dot(its.ns, -ray.dir)) : 0.f;
}

f32 ls::AreaLight::pdf(ls_Param_In const LightSampleRecord * refRec)
{
	Unimplement;
	return f32();
}

std::string ls::AreaLight::strOut() const
{
	
	return std::string();
}

ls::AreaLight::AreaLight(ParamSet & paramSet)
{
	mRadiance = paramSet.querySpectrum("radiance", 1.f);
}
