#pragma once
#include<config/config.h>
#include<config/lsPtr.h>
#include<config/declaration.h>
#include<config/module.h>
namespace ls
{
	class Camera:public Module
	{
	public:
		Camera(const std::string& id = "camera");
		virtual ~Camera();

		virtual CameraPtr copy() const { return nullptr; }

		virtual f32 spawnRay(ls_Param_In SamplerPtr sampler,
			ls_Param_In const CameraSample& sample,
			ls_Param_Out CameraSpwanRayRecord* rec) const = 0;

		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_In const IntersectionRecord& refIts,
			ls_Param_Out CameraSample* cameraSample,
			ls_Param_Out CameraSampleRecord* rec) const = 0;

		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_Out CameraSampleRecord* rec) const  = 0;

		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_In const CameraSample& sample,
			ls_Param_Out CameraSampleRecord* rec) const = 0;

		virtual f32 pdf(ls_Param_In const CameraSampleRecord* rec) const = 0;

		virtual Vec3 look() const = 0;

		//commit changes into camera
		virtual void commit() = 0;


		void addFilm(Film* film) { mFilm = film; }
		void deleteFilm() { mFilm = nullptr; }
		auto getFilm() { return mFilm; }

		
		MaterialPtr getDummyMaterial() const { return mMaterial; }

	protected:
		Film*				mFilm = nullptr;
		MaterialPtr			mMaterial = nullptr;
		
	};
}