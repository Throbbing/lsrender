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
//	Point3 pRaster = Point3(sample.pos.x, sample.pos.y, 0.f);
	Point3 pRaster = Point3(sample.pos.x * mInvWidth, sample.pos.y * mInvHeight, 0.f);
	Point3 pCamera = mR2C(pRaster);
	Ray ray = Ray(Point3(0, 0, 0), normalize(Vec3(pCamera)));


	{
	
	}

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

	mInvWidth = 1.f / w;
	mInvHeight = 1.f / h;
	
	f32 aspect = f32(w) / f32(h);

	const Vec2 filmSize = Vec2(w,h);
	const Vec2 cropSize = filmSize;
	const Vec2 cropOffset = Vec2(0.f, 0.f);

	Vec2 relSize((f32)cropSize.x / (f32)filmSize.x,
		(f32)cropSize.y / (f32)filmSize.y);
	Point2 relOffset((f32)cropOffset.x / (f32)filmSize.x,
		(f32)cropOffset.y / (f32)filmSize.y);


	Mat4x4 C2R =
		Transform::Mat4x4Perspective(mFov, mNear, mFar) *
		Transform::Mat4x4Translate(Vec3(-1.0f, -1.0f / aspect, 0.0f)) *
		Transform::Mat4x4Scale(Vector(-0.5f, -0.5f*aspect, 1.0f)) *
		Transform::Mat4x4Translate(Vector(-relOffset.x, -relOffset.y, 0.0f)) *
		Transform::Mat4x4Scale(Vector(1.0f / relSize.x, 1.0f / relSize.y, 1.0f));


	mR2C = C2R.inverse();


#if 0

	/* Precompute some data for importance(). Please
	look at that function for further details */
	Point min(m_sampleToCamera(Point(0, 0, 0))),
		max(m_sampleToCamera(Point(1, 1, 0)));

	m_imageRect.reset();
	m_imageRect.expandBy(Point2(min.x, min.y) / min.z);
	m_imageRect.expandBy(Point2(max.x, max.y) / max.z);
	m_normalization = 1.0f / m_imageRect.getVolume();

	/* Clip-space transformation for OpenGL */
	m_clipTransform = Transform::translate(
		Vector((1 - 2 * relOffset.x) / relSize.x - 1,
			-(1 - 2 * relOffset.y) / relSize.y + 1, 0.0f)) *
		Transform::scale(Vector(1.0f / relSize.x, 1.0f / relSize.y, 1.0f));
#endif
}
