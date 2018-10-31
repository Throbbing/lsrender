#include<sampler/randomsampler.h>

f32 ls::RandomSampler::next1D()
{

	return rng.UniformFloat();
}

ls::Point2 ls::RandomSampler::next2D()
{
	return Point2(rng.UniformFloat(), rng.UniformFloat());
}
