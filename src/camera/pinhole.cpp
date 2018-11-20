#include<camera\pinhole.h>
#include<function\log.h>
#include<film\film.h>
ls::Pinhole::Pinhole(const Transform & w2c, f32 shutterStart, f32 shutterEnd, f32 fov,
	f32 near,f32 far)
{
	mW2C = w2c;
	mC2W = mW2C.inverse();
	mShutterStart = shutterStart;
	mShutterEnd = shutterEnd;
	mFov = fov;
	mNear = near;
	mFar = far;

	mC2S = Transform::Mat4x4Perspective(mFov, mNear, mFar);
	mS2C = mC2S.inverse();


	

}

f32 ls::Pinhole::spawnRay(ls_Param_In const Sampler * sampler, ls_Param_In const CameraSample & sample, ls_Param_Out Record * rec) const
{
	Unimplement
	return f32();
}

void ls::Pinhole::sample(ls_Param_In const Sampler * sampler, ls_Param_Out Record * rec) const
{
	Unimplement
}

f32 ls::Pinhole::pdf(ls_Param_In const Record * rec) const
{
	Unimplement
	return f32();
}

void ls::Pinhole::commit() 
{
	if (!mFilm)
		ls_AssertMsg(false, "Lack Film in Pinhole");

	s32 width = mFilm->getWidth();
	s32 height = mFilm->getHeight();
	
	f32 frame = f32(width) / f32(height);

	Point2 pMin, pMax;
	if (frame > 1.f) {
		pMin.x = -frame;
		pMax.x = frame;
		pMin.y = -1.f;
		pMax.y = 1.f;
	}
	else {
		pMin.x = -1.f;
		pMax.x = 1.f;
		pMin.y = -1.f / frame;
		pMax.y = 1.f / frame;
	}
	

	mS2R = 
		Transform::Mat4x4Scale(width,height,1) *
		Transform::Mat4x4Scale(1 / (pMax.x - pMin.x),
			1 / (pMin.y - pMax.y), 1) *
		Transform::Mat4x4Translate(Vec3(-pMin.x, -pMax.y, 0));
		
	mR2S = mS2R.inverse();
	mR2C = mC2S.getMat().inverse() * mR2S.getMat();

}
