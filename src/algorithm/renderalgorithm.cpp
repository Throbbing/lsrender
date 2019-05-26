#include<algorithm\renderalgorithm.h>
#include<config/common.h>
#include<math\math.h>
#include<function\log.h>
#include<camera\camera.h>
#include<film\film.h>
#include<sampler\sampler.h>
#include<scene/scene.h>
#include<record\record.h>
#include<function\rng.h>
void ls::RenderAlgorithm::render(ScenePtr scene,
	SceneRenderBlock* sceneRenderBlock,
	SamplerPtr sampler, 
	CameraPtr camera, RNG & rng) const
{
		
	auto film = camera->getFilm();
	s32 spp = lsRender::sampleInfo.spp;

	for (s32 h = sceneRenderBlock->yStart; h < sceneRenderBlock->yEnd ; ++h)
	{
		for (s32 w = sceneRenderBlock->xStart; w < sceneRenderBlock->xEnd ; ++w)
		{
//			if (w != 640 || h != 84)
//				continue;


			for (s32 i = 0; i < spp; ++i)
			{
				CameraSample cs;
				auto offset = sampler->next2D();
				cs.pos.x = w + offset.x;
				cs.pos.y = h + offset.y;


				CameraSpwanRayRecord csrRec;
				camera->spawnRay(sampler, cs, &csrRec);

				CameraSampleRecord cr;
				cr.samplePosition = csrRec.spwanRay.ori;
				cr.sampleDirection = csrRec.spwanRay.dir;
				auto l = Li(csrRec.spwanRay, 0, &cr, scene, sampler, rng, nullptr);
				//			auto l = csrRec.ray.dir * 0.5f + Vec3(0.5f);
				//			film->addPixel(Spectrum(l.x,l.y,l.z), w, h);
				film->addPixel(l, w, h);
			}
		}
	}
	

}


