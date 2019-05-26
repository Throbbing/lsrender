#include<config/lsPtr.h>
#include<algorithm/path.h>
#include<record/record.h>
#include<function/stru.h>
#include<scene/scene.h>
#include<light/light.h>
#include<material/material.h>
#include<resource/xmlHelper.h>
#include<resource/resourceManager.h>
#include<sampler/sampler.h>
#include<spectrum/spectrum.h>
#include<scatter/scatter.h>



/*
	Render equation in path space 
	
	$$ I_j=\prod_{k=2}^ {\infty} \int_{\Omega_k} f_j(\overline{P_k})d\mu(\overline{P_k})$$

	$$f_j(\overline{P_k}) = Le(P_k\to P_{k-1})\prod_{i=1}^{k-1}(f(p_{i+1}\to p_i \to p_{i-1})G(p_{i+1},p_i)) * G(p_1,p_0)W_e(p_0 \to p_1)d\mu(\overline{P_k})$$
*/

ls::RenderAlgorithmPtr ls::PathTracer::copy() const
{
	ParamSet paramSet = ParamSet("renderAlgorithm", "path", "path", "path");
	paramSet.adds32("maxDepth", mPathMaxDepth);

	return ResourceManager::createAlgorithm(paramSet);
}

	ls::Spectrum ls::PathTracer::Li(ls_Param_In const DifferentialRay ray,
	ls_Param_In s32 depth,
	ls_Param_In CameraSampleRecord* csr,
	ls_Param_In Scene* scene,
	ls_Param_In Sampler* sampler,
	ls_Param_In RNG& rng,
	ls_Param_In MemoryAllocater* arena) const
{
	
	
	
#if 1

	ls::Spectrum L =  ls::Spectrum(0.f);
	ls::Spectrum throughput = ls::Spectrum(1.f);
	
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


	auto castRay = cameraRay;
	while (castRay.depth < mPathMaxDepth)
	{
		if (throughput.isBlack())
			break;

		//intersection on the arealight,the path will terminate
		//Note that  only depth = 0(i.e first sample vertex are on light source) ,we will take arealight into radiance
		//since all of arealight samples will be  processed in "bsdf sample" except the first vertex
		
		if (itsRec.areaLight)
		{
			if (castRay.depth == 0)
			{
				auto areaLight = itsRec.areaLight;
				L += areaLight->sample(castRay);
				
			}
			break;
		}
		
		/*
			estimate the path integral by this formula
			$$\frac {Le(p_k \to p_{k-1})\prod_{i=1}^{k-1}f(p_{i+1} \to p_i \to p_{i-1})G(p_{i+1},p_i)G(p_1.p_0)W_e(p_0 \to p_1)}{p_A(\overline{P_k]})}$$

			
			$$\frac  {f(p_{i+1} \to p_i \to p_{i-1})G(p_{i+1},p_i)}{p_A(p_i \to p_{i+1})}$$

			
			$$\frac {f* \frac { \cos\theta_i  \cos\theta_{i+1}}{r^2}}{p_\omega \frac{\cos\theta_{i+1}}{r^2}} $$
			$$\frac {f*\cos\theta_i}{p_\omega}$$
		*/



		/*
			 Direct light calculation 
			 However, there is no possiblity to sample bsdf which has 'S' component
		*/
		{
			auto bsdf = itsRec.material->getSurfaceScattering();
			if (!bsdf)
				break;

			if (!bsdf->isDelta())
			{
				//Sample a light source from scene
				LightSampleRecord lsRec;
				auto selectPdf = scene->sampleLight(sampler, &lsRec);
				auto light = lsRec.light;


				//Sample Le and direction from selected light source
				clearMemory(&lsRec, sizeof(lsRec));
				light->sample(sampler, &itsRec, &lsRec);
				auto le = lsRec.le;
				auto lightPoint = lsRec.samplePosition;
				auto wi = -lsRec.sampleDirection;
				auto lightPdfW = lsRec.pdfW * selectPdf;

				//calculate bsdf
				SurfaceSampleRecord surSRec;
				
				RenderLib::fillScatteringRecordForBSDFValueAndPdf(
					itsRec.position,
					itsRec.ns,
					-castRay.dir,
					wi,
					TransportMode::ETransport_Radiance,
					&surSRec);

				RenderLib::surfaceBSDFValueAndPdf(bsdf, &surSRec);

				auto bsdfPdfW = surSRec.pdf;
				auto bsdfVal = surSRec.sampledValue * itsRec.material->scatteringFactor(surSRec,itsRec);

				auto visible = RenderLib::visible(scene, lightPoint, itsRec.position);

				if (light->isDelta()) bsdfPdfW = 0.f;

				if (visible && !bsdfVal.isBlack() && dot(surSRec.normal,wi) > 0.f &&
					 !lsMath::closeZero(lightPdfW))
				{
					L += throughput * le *bsdfVal * dot(surSRec.normal, wi) * RenderLib::mis(lightPdfW, bsdfPdfW) /
						lightPdfW;
				}



			}//endif !bsdf->isDelta()
		}//Direct Light Calculation

		/*
			BSDF Sample
		*/
		{
			auto bsdf = itsRec.material->getSurfaceScattering();
			if (!bsdf)
				break;

			//sample direction from bsdf
			SurfaceSampleRecord surSRec;
			RenderLib::fillScatteringRecordForBSDFSample(itsRec.position,
				itsRec.ns,
				-castRay.dir,
				TransportMode::ETransport_Radiance,
				&surSRec);

			RenderLib::sampleSurfaceBSDF(sampler, bsdf, &surSRec);

			auto bsdfPdfW = surSRec.pdfRadiance;
			auto bsdfVal = surSRec.sampledValue * itsRec.material->scatteringFactor(surSRec,itsRec);
			auto wi = surSRec.wi;

			if (lsMath::closeZero(bsdfPdfW) || bsdfVal.isBlack())
				break;
			
			//construct  a new ray
			castRay = DifferentialRay(surSRec.position, wi, castRay.depth + 1,castRay.time);


			bool hitLight = false;
			Light* light = nullptr;
			//cast a new ray to detect whether hitting the light source or not
			//we dont have to consider the singular light source since such lights have delta distributions
			if (scene->intersect(castRay, &itsRec))
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
				break;
			}

			if (hitLight && light)
			{
				LightSampleRecord lsRec;
				auto le = light->sample(castRay);
				auto lightPdfW = !(bsdf->isDelta()) ? light->pdf(castRay) : 0.f;

				

				if (!le.isBlack())
				{
					L += throughput * le * bsdfVal * std::fabs(dot(surSRec.normal, wi)) * RenderLib::mis(bsdfPdfW, lightPdfW)
						/ bsdfPdfW;
				}
			}
			
			//update throughput
			throughput *= bsdfVal * std::fabs(dot(surSRec.normal, wi)) / bsdfPdfW;

		}//end BSDF Sample
		
		/*
			The previous k-1 vertices are reused when constructing a new path
			we use the local sample to create a new vertex
		*/
		

		//Russian roulette
#if 0
		{
			//q is the probability that the path is continued ,while 1- q represent that the path will terminate
			f32 q = std::min(0.95f, throughput.luminance());

			if (sampler->next1D() < q)
			{
				throughput /= q;
			}
			else
			{
				break;
			}
		}//russian roulette

#endif
		
		
	}//end while(castRay.depth <= mPathMaxDepth)

	return L;
#endif
	
}

std::string ls::PathTracer::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Render Algorithm: " << "Path" << std::endl;
	oss << "MaxDepth        : " << mPathMaxDepth << std::endl;
	oss << "RouletteDepth   : " << mPathMaxDepth << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::PathTracer::PathTracer(ParamSet & paramSet)
{
	mPathMaxDepth = paramSet.querys32("maxDepth", 1);
}
