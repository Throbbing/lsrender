#pragma once

#include<config/config.h>
#include<config/declaration.h>
#include<spectrum/spectrum.h>
namespace ls
{
	class Light
	{
		friend Scene;
	public:
		Light() {}
		virtual ~Light() {}

		virtual bool isDelta() = 0;
		virtual bool getPower() = 0;

		virtual void attachMesh(std::shared_ptr<Mesh> mesh) { mAttachedMesh = mesh; }
		virtual void detachMesh() { mAttachedMesh = nullptr; }

		virtual void sample(ls_Param_In Sampler* sampler,
			ls_Param_Out Record* rec) = 0;
		virtual void sample(ls_Param_In Sampler* sampler,
			ls_Param_In const Record* refRec,
			ls_Param_Out Record* rec) = 0;

		virtual ls::Spectrum sample(const Ray& ray) { return ls::Spectrum(0); }
		virtual f32			 pdf(const Ray& ray) { return 0.f; }

		virtual f32 pdf(ls_Param_In const Record* refRec) = 0;

	protected:
		std::shared_ptr<Mesh> mAttachedMesh = nullptr;
	};
}