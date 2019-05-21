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

		virtual SamplerPtr copy() const override;
		f32 next1D() override;
		Point2 next2D() override;

		virtual void commit() override {};
		virtual std::string strOut() const override;

	private:
		RNG			rng;
	};
}