#include<algorithm\renderalgorithm.h>
#include<math\math.h>
#include<function\log.h>
#include<camera\camera.h>
#include<film\film.h>
#include<sampler\sampler.h>
#include<record\record.h>
#include<function\rng.h>
void ls::RenderAlgorithm::render(Scene * scene,
	Sampler * sampler, 
	Camera * camera, RNG & rng)const
{
		
	auto film = camera->getFilm();
	s32 width = film->getWidth();
	s32 height = film->getHeight();

	for (s32 h = 0; h < height; ++h)
	{
		for (s32 w = 0; w < width; ++w)
		{
			CameraSample cs;
			cs.pos.x = w;
			cs.pos.y = h;

			
			CameraSpwanRayRecord csrRec;
			camera->spawnRay(sampler, cs, &csrRec);

			CameraSampleRecord cr;
			cr.samplePosition = csrRec.ray.ori;
			cr.sampleDirection = csrRec.ray.dir;
			auto l = Li(&cr, scene, sampler, rng, nullptr);
//			auto l = csrRec.ray.dir * 0.5f + Vec3(0.5f);
//			film->addPixel(Spectrum(l.x,l.y,l.z), w, h);
			film->addPixel(l, w, h);
		}
	}
	
	film->flush();

}


