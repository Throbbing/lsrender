#include<math/frame.h>
#include<function/func.h>
#include<scatter/distribution.h>
#include<spectrum/spectrum.h>
#include<sampler/sampler.h>


f32 ls::Distribution::sample(ls_Param_In SamplerPtr sampler, 
	ls_Param_In const Vec3 & w, 
	ls_Param_Out Vec3 * wh,
	ls_Param_Out f32 * pdfH) const
{
	auto uv = sampler->next2D();

	f32 value = 0.f;
	
	switch (mDistributionType)
	{
	case ls::EDistribution_Beckman:
		if (mSampleAllDistribution) MonteCarlo::sampleBeckmanDistributionAll(uv, mAlphaU, mAlphaV, wh, pdfH);
		else MonteCarlo::sampleBeckmanDistributionVisible(uv, mAlphaU, mAlphaV, wh, pdfH);
		value = RenderLib::beckmanDistribution(*wh, mAlphaU, mAlphaV);

		break;
	case ls::EDistribution_GGX:
		if (mSampleAllDistribution) MonteCarlo::sampleGGXDistributionAll(uv, mAlphaU, mAlphaV, wh, pdfH);
		else MonteCarlo::sampleGGXDistributionVisible(uv, mAlphaU, mAlphaV, wh, pdfH);
		value = RenderLib::ggxDistribution(*wh, mAlphaU, mAlphaV);

		break;
	default:
		if (mSampleAllDistribution) MonteCarlo::sampleGGXDistributionAll(uv, mAlphaU, mAlphaV, wh, pdfH);
		else MonteCarlo::sampleGGXDistributionVisible(uv, mAlphaU, mAlphaV, wh, pdfH);
		value = RenderLib::ggxDistribution(*wh, mAlphaU, mAlphaV);
		break;
	}
	return value;
	
}

f32 ls::Distribution::D(ls_Param_In const Vec3 & wi, ls_Param_In const Vec3 & wo) const
{
	return D(normalize(wi + wo));
}

f32 ls::Distribution::D(ls_Param_In const Vec3 & wh) const
{
	switch (mDistributionType)
	{
	case EDistribution_Beckman:
		return RenderLib::beckmanDistribution(wh, mAlphaU, mAlphaV);
		break;
	case EDistribution_GGX:
		return RenderLib::ggxDistribution(wh, mAlphaU, mAlphaV);
		break;
	default:
		return RenderLib::ggxDistribution(wh, mAlphaU, mAlphaV);
		break;
	}
}

f32 ls::Distribution::G(ls_Param_In const Vec3 & wi, ls_Param_In const Vec3 & wh)
{
	auto cosPhi = Frame::cosPhi(wi);
	auto sinPhi = Frame::sinPhi(wi);
	auto cosPhi2 = cosPhi * cosPhi;
	auto sinPhi2 = sinPhi * sinPhi;

	auto alpha = std::sqrt(cosPhi2 * mAlphaU * mAlphaU + sinPhi2 * mAlphaV * mAlphaV);

	if (mDistributionType == EDistribution_Beckman)
		return RenderLib::beckmanG1(wi, wh, alpha);
	else
		return RenderLib::ggxG1(wi, wh, alpha);
}

f32 ls::Distribution::G(ls_Param_In const Vec3 & wi, ls_Param_In const Vec3 & wo, ls_Param_In const Vec3 & wh)
{
	return G(wi, wh) * G(wo, wh);
}

f32 ls::Distribution::pdf(ls_Param_In const Vec3 & w, ls_Param_In const Vec3 & wh) const
{
	if (mSampleAllDistribution)
	{
		switch (mDistributionType)
		{
		case ls::EDistribution_Beckman:
			return MonteCarlo::beckmanDistributionAllPdf(w, wh, mAlphaU, mAlphaV);
			break;
		case ls::EDistribution_GGX:
			return MonteCarlo::ggxDistributionAllPdf(w, wh, mAlphaU, mAlphaV);
			break;
		default:
			return MonteCarlo::ggxDistributionAllPdf(w, wh, mAlphaU, mAlphaV);
			break;
		}
	}
	else
	{
		switch (mDistributionType)
		{
		case ls::EDistribution_Beckman:
			return MonteCarlo::beckmanDistributionVisiblePdf(w, wh, mAlphaU, mAlphaV);
			break;
		case ls::EDistribution_GGX:
			return MonteCarlo::ggxDistributionVisiblePdf(w, wh, mAlphaU, mAlphaV);
			break;
		default:
			return MonteCarlo::ggxDistributionVisiblePdf(w, wh, mAlphaU, mAlphaV);
			break;
		}
	}
}

