#include<sampler/randomsampler.h>

f32 ls::RandomSampler::next1D()
{

	return rng.UniformFloat();
}

ls::Point2 ls::RandomSampler::next2D()
{
	return Point2(rng.UniformFloat(), rng.UniformFloat());
}

std::string ls::RandomSampler::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Sampler: " << "Random" << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}
