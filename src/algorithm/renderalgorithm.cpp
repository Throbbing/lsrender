#include<algorithm\renderalgorithm.h>
#include<math\math.h>
#include<function\log.h>
#include<camera\camera.h>
#include<film\film.h>
#include<sampler\sampler.h>
#include<record\record.h>
#include<function\rng.h>
void ls::RenderAlgorithm::render(ScenePtr scene,
	SamplerPtr sampler, 
	CameraPtr camera, RNG & rng) const
{
		
	auto film = camera->getFilm();
	s32 width = film->getWidth();
	s32 height = film->getHeight();
	s32 spp = 512;

	for (s32 h = 0; h < height ; ++h)
	{
		for (s32 w = 0; w < width ; ++w)
		{

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
	
	film->flush();

}


