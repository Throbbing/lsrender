#include<camera\pinhole.h>
#include<function\log.h>
#include<film\film.h>
#include<record\record.h>
#include<sampler\sampler.h>
#include<function\stru.h>
ls::Pinhole::Pinhole(const Transform & c2w, f32 shutterStart, f32 shutterEnd, f32 fov,
	f32 near,f32 far)
{
	mC2W = c2w;
	mW2C = c2w.inverse();
	mShutterStart = shutterStart;
	mShutterEnd = shutterEnd;
	mFov = fov;
	mNear = near;
	mFar = far;

	


	

}

f32 ls::Pinhole::spawnRay(ls_Param_In const Sampler * sampler, 
	ls_Param_In const CameraSample & sample, 
	ls_Param_Out CameraSpwanRayRecord * rec) const
{
	Point3 pRaster = Point3(sample.pos.x, sample.pos.y, 0.f);
	Point3 pCamera = mR2C(pRaster);
	Ray ray = Ray(Point3(0, 0, 0), normalize(Vec3(pCamera)));

	ray = mC2W(ray);
	ray.dir = normalize(ray.dir);
	rec->ray = DifferentialRay(ray);

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

	f32 w = mFilm->getWidth();
	f32 h = mFilm->getHeight();
	
	f32 aspect = f32(w) / f32(h);

	f32 invtan = 1.f / std::tanf(lsMath::degree2Radian(mFov*0.5f));
	mC2S = { (1.f / aspect)*invtan, 0.f, 0.f, 0.f,
		0.f, invtan, 0.f, 0.f,
		0.f, 0.f, mFar / (mFar - mNear), 1.f,
		0.f, 0.f, -mNear*mFar / (mFar - mNear), 0.f };

	mS2C = mC2S.inverse();

	mS2R = Mat4x4{ w*0.5f, 0.f, 0.f, 0.f,
		0.f, -h*0.5f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		w*0.5f, h*0.5f, 0.f, 1.f };

	mR2S = mS2R.getMat().inverse();

	mR2C = (mC2S.getMat() * mS2R.getMat()).inverse();

}
