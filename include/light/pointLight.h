#pragma once
#include<light/light.h>

namespace ls
{
	class PointLight :public Light
	{
		friend ResourceManager;
	public:
		PointLight(const Point3& position,
			const Spectrum& intensity = Spectrum(0.f)):
			mPosition(position),
			mIntensity(intensity){}
		virtual ~PointLight() {}

		virtual LightType getLightType() const override { return ELight_Point; }

		virtual bool isDelta()		override	{ return true; };
		virtual Spectrum getPower()	override	{ return mIntensity * lsMath::PI_4; };

		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_Out LightSampleRecord* rec) override;
	
		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_In const IntersectionRecord* refRec,
			ls_Param_Out LightSampleRecord* rec) override;

		virtual ls::Spectrum sample(const Ray& ray) override { return ls::Spectrum(0); }
		virtual f32			 pdf(const Ray& ray)	override { return 0.f; }

		virtual f32 pdf(ls_Param_In const LightSampleRecord* refRec) override;


		void applyPosition(const Point3& position) { mPosition = position; }
		void applyIntensity(const Spectrum& intensity) { mIntensity = intensity; }


		virtual void commit() {};
		virtual std::string strOut() const override;

	protected:
		PointLight(ParamSet& paramSet);
	protected:
		Point3			mPosition;
		Spectrum		mIntensity;
	};
}