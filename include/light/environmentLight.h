#pragma once

#include<Light/light.h>
#include<function/func.h>
#include<math/transform.h>
namespace ls
{
	class EnvironmentLight:public Light
	{
		friend ResourceManager;
	public:
		EnvironmentLight(const std::string& filename,
			f32 scale);

		virtual ~EnvironmentLight() {}

		virtual LightType getLightType() const override { return ELight_Environment; };

		virtual bool isDelta()		override { return false; };
		
		
		virtual Spectrum getPower()	override ;

		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_Out LightSampleRecord* rec) override;

		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_In const IntersectionRecord* refRec,
			ls_Param_Out LightSampleRecord* rec) override;

		virtual ls::Spectrum sample(const Ray& ray) override;
		virtual f32			 pdf(const Ray& ray)	override;

		virtual f32 pdf(ls_Param_In const LightSampleRecord* refRec) override;



		virtual void commit() {};
		virtual std::string strOut() const override;

		

	protected:
		EnvironmentLight(ParamSet& paramSet);
		s32 fetch(s32 x, f32 y) const
		{
			return y * mWidth + x;
		}

	private:

		Point2 dir2UVAndTheta(const Vec3& dir, f32* theta = nullptr) const;

		Vec3 uv2DirAndTheta(const Point2& uv, f32 * theta = nullptr) const;

		Spectrum fetch(Point2 uv) const;

		std::shared_ptr<Distribution2D>  mEnvDistribution;
		s32								 mWidth;
		s32								 mHeight;
		std::vector<Spectrum>			 mData;
		f32								 mScale;
		Transform						 mL2W;
		Transform						 mW2L;
	};
}