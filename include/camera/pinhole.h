#pragma once
#include<config\config.h>
#include<config\declaration.h>
#include<math\frame.h>
#include<math\transform.h>
#include<camera\camera.h>
namespace ls
{
	class Pinhole:public Camera
	{
	public:
		Pinhole(const Transform& c2w,
			f32 shutterStart, f32 shutterEnd,
			f32 fov,
			f32 near,
			f32 far);

		virtual ~Pinhole() {}

		virtual f32 spawnRay(ls_Param_In const Sampler* sampler,
			ls_Param_In const CameraSample& sample,
			ls_Param_Out CameraSpwanRayRecord* rec) const override;

		virtual void sample(ls_Param_In const Sampler* sampler,
			ls_Param_Out CameraSampleRecord* rec) const override;

		virtual f32 pdf(ls_Param_In const CameraSampleRecord* rec) const override ;

		//commit changes into camera
		virtual void commit()  override;
		


	protected:
		Transform			mW2C, mC2W;
		Transform			mC2S, mS2C;
		Transform			mS2R, mR2S;
		Transform			mR2C;
		f32					mShutterStart;
		f32					mShutterEnd;
		f32					mFov;
		f32					mNear;
		f32					mFar;

		f32					mInvWidth;
		f32					mInvHeight;
	};
}