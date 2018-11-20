#pragma once
#include<config/config.h>
#include<config/declaration.h>

namespace ls
{
	class Camera
	{
	public:
		Camera() {}
		virtual ~Camera() {}

		virtual f32 spawnRay(ls_Param_In const Sampler* sampler,
			ls_Param_In const CameraSample& sample,
			ls_Param_Out Record* rec) const = 0;

		virtual void sample(ls_Param_In const Sampler* sampler,
			ls_Param_Out Record* rec) const  = 0;

		virtual f32 pdf(ls_Param_In const Record* rec) const = 0;

		//commit changes into camera
		virtual void commit() = 0;


		void addFilm(Film* film) { mFilm = film; }
		void deleteFilm() { mFilm = nullptr; }
		auto getFilm() { return mFilm; }

		
		

	protected:
		Film*				mFilm = nullptr;
		
	};
}