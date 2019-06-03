#pragma once
#include<light/light.h>

namespace ls
{
	class AreaLight :public Light
	{
		friend ResourceManager;
	public:
		AreaLight() {}
		virtual ~AreaLight() {}

		virtual LightType getLightType() const override { return ELight_Area; }

		virtual bool isDelta()		override { return false; };
		virtual Spectrum getPower()	override ;

		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_Out LightSampleRecord* rec) override;

		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_In const IntersectionRecord* refRec,
			ls_Param_Out LightSampleRecord* rec) override;

		virtual ls::Spectrum sample(const Ray& ray) override;
		virtual ls::Spectrum sample(const Ray& ray, const IntersectionRecord& its) override;
		virtual f32			 pdf(const Ray& ray)	override;
		virtual f32			 pdf(const Ray& ray ,const IntersectionRecord& its) override;

		virtual f32 pdf(ls_Param_In const LightSampleRecord* refRec) override;


		void attachMesh(MeshPtr mesh)
		{
			mMesh = mesh;
		}

		virtual void commit() {};
		virtual std::string strOut() const override;

	protected:
		AreaLight(ParamSet& paramSet);
	protected:
		MeshPtr			mMesh = nullptr;
		Spectrum		mRadiance;
	};
}