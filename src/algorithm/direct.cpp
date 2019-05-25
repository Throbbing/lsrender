#include<algorithm/direct.h>

#include<function/stru.h>

#include<light/light.h>
#include<material/material.h>
#include<record/record.h>
#include<scene/scene.h>
#include<sampler/sampler.h>
#include<spectrum/spectrum.h>
#include<scatter/scatter.h>
#include<resource/xmlHelper.h>
#include<resource/resourceManager.h>

ls::RenderAlgorithmPtr ls::DirectTracer::copy() const
{
	ParamSet paramSet = ParamSet("renderAlgorithm", "direct", "direct", "direct");

	paramSet.adds32("maxDepth", mMaxDepth);
	return ResourceManager::createAlgorithm(paramSet);
}

ls::Spectrum ls::DirectTracer::Li(ls_Param_In const DifferentialRay ray,
	ls_Param_In	s32					 depth,
	ls_Param_In CameraSampleRecord * cameraSampleRec, 
	ls_Param_In ScenePtr scene, 
	ls_Param_In SamplerPtr sampler, 
	ls_Param_In RNG & rng, 
	ls_Param_In MemoryAllocaterPtr arena) const
{
	if (depth >= mMaxDepth)
		return Spectrum(0.f);

	ls::Spectrum L = ls::Spectrum(0.f);
	//spawn new ray
	DifferentialRay cameraRay(ray);

	IntersectionRecord itsRec;
	if (!scene->intersect(cameraRay, &itsRec))
	{
		//we need take radiance from environment light into account when ray hit nothing
		auto envLight = scene->envrionmentLight();
		if (envLight)
		{
			L = envLight->sample(cameraRay);
		}
		return L;
	}

	//Hit Area Light
	if (itsRec.areaLight)
	{
		return itsRec.areaLight->sample(cameraRay);
	}


	auto bsdf = itsRec.material->getSurfaceScattering();

	if (!bsdf)
		return Spectrum(0.f);


	//Sample a light source from scene
	LightSampleRecord lsRec;
	auto selectPdf = scene->sampleLight(sampler, &lsRec);
	auto light = lsRec.light;

	/*
		Light Sample
	*/
	if (!bsdf->isDelta())
	{
		//Sample Le and direction from selected light source
		clearMemory(&lsRec, sizeof(lsRec));
		light->sample(sampler, &itsRec, &lsRec);
		auto le = lsRec.le;
		auto lightPoint = lsRec.samplePosition;
		auto wi = -lsRec.sampleDirection;
		auto lightPdfW = lsRec.pdfW * selectPdf;

		//calculate bsdf
		ScatteringRecord surSRec;
		surSRec.transportMode = ETransport_Radiance;
		surSRec.scatterFlag = bsdf->scatteringFlag();
		surSRec.wo = -cameraRay.dir;
		surSRec.wi = wi;
		surSRec.position = itsRec.position;
		surSRec.normal = itsRec.ns;
		
		RenderLib::surfaceBSDFValueAndPdf(bsdf, &surSRec);

		auto bsdfPdfW = surSRec.pdf;
		auto bsdfVal = surSRec.sampledValue * itsRec.material->scatteringFactor(surSRec,itsRec);

		auto visible = RenderLib::visible(scene, itsRec.position, lightPoint);

		if (light->isDelta()) bsdfPdfW = 0.f;

		if (visible && !bsdfVal.isBlack() && dot(surSRec.normal, wi) > 0.f 
			&& !lsMath::closeZero(lightPdfW))
		{

			L += le  * bsdfVal * dot(surSRec.normal, wi) * RenderLib::mis(lightPdfW, bsdfPdfW) /
				lightPdfW;
		}
	}//endif !bsdf->isDelta()

	/*
		BSDF Sample
	*/
	ScatteringRecord specularSurfaceRec;
	if (!lsRec.light->isDelta())
	{

		//sample direction from bsdf
		ScatteringRecord surSRec;
		surSRec.position = itsRec.position;
		surSRec.normal = itsRec.ns;
		surSRec.transportMode = ETransport_Radiance;
		surSRec.wo = -cameraRay.dir;
		RenderLib::sampleSurfaceBSDF(
			sampler, 
			bsdf,
			&surSRec);

		auto bsdfPdfW = surSRec.pdf;
		auto bsdfVal = surSRec.sampledValue * itsRec.material->scatteringFactor(surSRec,itsRec);

		auto wi = surSRec.wi;



		//construct  a new ray
		auto castRay = DifferentialRay(surSRec.position, wi, cameraRay.depth + 1, cameraRay.time);


		bool hitLight = false;
		Light* light = nullptr;
		//cast a new ray to detect whether hitting the light source or not
		//we dont have to consider the singular light source since such lights have delta distributions
		
		//直接光照中，不处理有Scattering_S材质由递归来处理
		if (!RenderLib::matchScatterFlag(bsdf,
			EScattering_S | EScattering_Surface)
			&& scene->intersect(castRay, &itsRec))
		{
			auto areaLight = itsRec.areaLight;
			if (areaLight && !areaLight->isDelta())
			{
				hitLight = true;
				light = areaLight;
			}

		}
		else
		{
			//there may be a environment light in scene
			if (scene->envrionmentLight())
			{
				hitLight = true;
				light = scene->envrionmentLight();
			}
		}

		if (hitLight && light)
		{
			LightSampleRecord lsRec;
			auto le = light->sample(cameraRay);
			auto lightPdfW = !bsdf->isDelta() ? light->pdf(cameraRay) : 0.f;

			if (!le.isBlack())
			{
				L += le * bsdfVal * std::fabs(dot(surSRec.normal, wi)) * RenderLib::mis(bsdfPdfW, lightPdfW)
					/ bsdfPdfW;
			}
		}

	}//end !lsRec.light->isDelta()

#if 0
	//处理EScattering_S
	if (RenderLib::matchScatterFlag(bsdf, EScattering_S | EScattering_Surface))
	{
		if (specularSurfaceRec.scatterFlag & EScattering_Reflection)
		{
			L += itsRec.material->reflectance(itsRec) *
				specularSurfaceRec.sampleValue * Li(cameraRay, depth + 1,
				cameraSampleRec,
				scene,
				sampler,
				rng, arena) / specularSurfaceRec.pdf;
		}
		else
		{
			L += itsRec.material->transmittance(itsRec) *
				specularSurfaceRec.sampleValue * Li(cameraRay, depth + 1,
					cameraSampleRec,
					scene,
					sampler,
					rng, arena) / specularSurfaceRec.pdf;
		}
	}
#endif 
	return L;


}

std::string ls::DirectTracer::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Render Algorithm: " << "Direct" << std::endl;
	oss << "MaxDepth        : " << mMaxDepth << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::DirectTracer::DirectTracer(ls::ParamSet& paramSet)
{
	mMaxDepth = paramSet.querys32("maxDepth", 1);
	
}
