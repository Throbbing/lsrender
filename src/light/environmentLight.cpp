#include<config/common.h>
#include<light/environmentLight.h>
#include<function/file.h>
#include<function/stru.h>
#include<resource/resourceManager.h>
#include<resource/xmlHelper.h>
#include<record/record.h>
#include<scene/scene.h>
#include<scatter/scatter.h>
#include<sampler/sampler.h>
#include<texture/ImageTexture.h>




ls::EnvironmentLight::EnvironmentLight(const std::string & filename, f32 scale)
{
	auto bitmap = ResourceManager::loadTextureFromFile(Path(filename));
	mWidth = bitmap.width;
	mHeight = bitmap.height;
	mScale = scale;

	mData.resize(mWidth * mHeight);
	for (s32 i = 0; i < mData.size(); ++i) mData[i] = bitmap.data[i];

	std::vector<f32> luminance(mWidth * mHeight);

	f32 invWidth = 1.f / mWidth;
	f32 invHeight = 1.f / mHeight;

	for (s32 h = 0; h < mHeight; ++h)
	{
		f32 theta = f32(lsMath::PI * (h + 0.5f) * invHeight);
		
		f32 sinTheta = std::sinf(theta);

		for (s32 w = 0; w < mWidth; ++w)
		{
			auto value = mData[h * mWidth + w].luminance();
			
			//p(w) = p(u,v)/( 2 * PI^2 * sinTheta)
			//��������ͼ���������ض���ͬʱ������ֱ�ӵõ�p(w)Ӧ��Ϊ��ֵ
			//�ڴ�����£�����p(u,v)��ֵ������p(w)��p(u,v)�Ĺ�ϵ
			//p(w)����sinTheta���
			//Ϊ��У�����������س���sinTheta,ʹ��p(u,v)��sinTheta���
			value *= sinTheta;
			
			luminance[h * mWidth + w] = value;
		}
	}

	mEnvDistribution = std::shared_ptr<Distribution2D>(new Distribution2D(&luminance[0], mWidth, mHeight));
}

ls::Spectrum ls::EnvironmentLight::getPower()
{
	Unimplement;
	return Spectrum();
}

void ls::EnvironmentLight::sample(ls_Param_In SamplerPtr sampler, ls_Param_Out LightSampleRecord * rec)
{
	Unimplement;
}

void ls::EnvironmentLight::sample(ls_Param_In SamplerPtr sampler, ls_Param_In const IntersectionRecord * refRec, ls_Param_Out LightSampleRecord * rec)
{
	auto sample = sampler->next2D();

	f32 uvPdf = 0.f;
	f32 theta;
	auto uv = mEnvDistribution->SampleContinuous(sample, &uvPdf);
	ls_Assert(uvPdf != 0.f);

	f32 worldDiameter = lsRender::scene->getWorldAABB().maxExtent();
	rec->sampleDirection = -uv2DirAndTheta(uv, &theta);
	rec->samplePosition = refRec->position - 2.f * worldDiameter * rec->sampleDirection;
	rec->le = fetch(uv);
	rec->mode = EMeasure_SolidAngle;
	rec->pdfW = uvPdf / (2.f * lsMath::PI * lsMath::PI * std::sinf(theta));
	rec->pdfA = 0.f;
	rec->pdfPos = 1.f;
	rec->light = LightPtr(this);	
}

ls::Spectrum ls::EnvironmentLight::sample(const Ray & ray)
{
	auto uv = dir2uv(ray.dir);
	return fetch(uv);
	
}

f32 ls::EnvironmentLight::pdf(const Ray & ray)
{
	//pdf (u,v)
	auto uv = dir2uv(ray.dir);
	auto theta = dir2Theta(ray.dir);
	auto sinTheta = std::cosf(theta);
	f32 uvPdf = mEnvDistribution->Pdf(Point2(uv));
	
	//p(w) = pdf(u,v)/ (2 * PI^2 * sinTheta)
	return uvPdf / (2.f * lsMath::PI * lsMath::PI * sinTheta);


}

f32 ls::EnvironmentLight::pdf(ls_Param_In const LightSampleRecord * refRec)
{
	//environment light ��ֻ�� ��������ǵ�Pdf
	f32 theta;
	auto uv = dir2UVAndTheta(-refRec->sampleDirection, &theta);

	f32 sinTheta = std::sinf(theta);
	f32 uvPdf = mEnvDistribution->Pdf(Point2(uv));

	return uvPdf / (2.f * lsMath::PI * lsMath::PI * sinTheta);

}

std::string ls::EnvironmentLight::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Light: " << "Environment" << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::Point2 ls::EnvironmentLight::dir2uv(const Vec3 & dir) const
{
	return Point2();
}

f32 ls::EnvironmentLight::dir2Theta(const Vec3 & dir) const
{
	return f32();
}

ls::Point2 ls::EnvironmentLight::dir2UVAndTheta(const Vec3 & dir, f32 * theta) const
{
	return Point2();
}

ls::Vec3 ls::EnvironmentLight::uv2DirAndTheta(const Point2 & uv, f32 * theta) const
{
	return Vec3();
}

ls::Spectrum ls::EnvironmentLight::fetch(Point2 uv) const
{
	//˫���Բ�ֵ
	uv.x *= f32(mWidth);
	uv.y *= f32(mHeight);

	s32 u0 = std::min(s32(uv.x), s32(mWidth - 1)); s32 u1 = std::min(u0 + 1, s32(mWidth - 1));
	s32 v0 = std::min(s32(uv.y), s32(mHeight - 1)); s32 v1 = std::min(v0 + 1, s32(mHeight - 1));

	f32 uOffset = uv.x - u0;
	f32 vOffset = uv.y - v0;

	return (1.f - uOffset) * (1.f - vOffset) * mData[fetch(u0, v0)] +
		(uOffset) * (1.f - vOffset) *mData[fetch(u1, v0)] +
		(1.f - uOffset) * (vOffset)* mData[fetch(u0, v1)] +
		(uOffset) * (vOffset)* mData[fetch(u1, v1)];
}

ls::EnvironmentLight::EnvironmentLight(ParamSet & paramSet)
{
	auto filename = paramSet.queryString("filename");
	auto scale = paramSet.queryf32("scale",1.f);

	auto bitmap = ResourceManager::loadTextureFromFile(Path(filename));
	mWidth = bitmap.width;
	mHeight = bitmap.height;
	mScale = scale;

	mData.resize(mWidth * mHeight);
	for (s32 i = 0; i < mData.size(); ++i) mData[i] = bitmap.data[i];

	std::vector<f32> luminance(mWidth * mHeight);

	f32 invWidth = 1.f / mWidth;
	f32 invHeight = 1.f / mHeight;

	for (s32 h = 0; h < mHeight; ++h)
	{
		f32 theta = f32(lsMath::PI * (h + 0.5f) * invHeight);

		f32 sinTheta = std::sinf(theta);

		for (s32 w = 0; w < mWidth; ++w)
		{
			auto value = mData[h * mWidth + w].luminance();

			//p(w) = p(u,v)/( 2 * PI^2 * sinTheta)
			//��������ͼ���������ض���ͬʱ������ֱ�ӵõ�p(w)Ӧ��Ϊ��ֵ
			//�ڴ�����£�����p(u,v)��ֵ������p(w)��p(u,v)�Ĺ�ϵ
			//p(w)����sinTheta���
			//Ϊ��У�����������س���sinTheta,ʹ��p(u,v)��sinTheta���
			value *= sinTheta;

			luminance[h * mWidth + w] = value;
		}
	}

	mEnvDistribution = std::shared_ptr<Distribution2D>(new Distribution2D(&luminance[0], mWidth, mHeight));

}
