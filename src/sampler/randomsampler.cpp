#include<sampler/randomsampler.h>
#include<resource/xmlHelper.h>
#include<resource/resourceManager.h>
ls::SamplerPtr ls::RandomSampler::copy() const
{
	ParamSet paramSet = ParamSet("sampler", "randomSampler", "1", "1");
	return ResourceManager::createSampler(paramSet);

}

f32 ls::RandomSampler::next1D()
{

	return rng.UniformFloat();
}

ls::Point2 ls::RandomSampler::next2D()
{
//	return Point2(0.5f, 0.5f);
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
