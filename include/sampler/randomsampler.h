#pragma once

#include<sampler/sampler.h>
#include<function/rng.h>
namespace ls
{
	class RandomSampler :public Sampler
	{
	public:
		RandomSampler() {}
		virtual ~RandomSampler() {}

		f32 next1D() override;
		Point2 next2D() override;

	private:
		RNG			rng;
	};
}