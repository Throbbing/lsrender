#include<config/declaration.h>
#include<camera/pinhole.h>
#include<function/log.h>
#include<film/film.h>
#include<record/record.h>
#include<resource/xmlHelper.h>
#include<resource/resourceManager.h>
#include<sampler/sampler.h>

#include<function/stru.h>
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



ls::CameraPtr ls::Pinhole::copy() const
{
	ParamSet paramSet("camera", "pinhole", "", "");
	paramSet.addf32("fov", mFov);
	paramSet.addTransform("c2w", mC2W);
	paramSet.addf32("nearZ", mNear);
	paramSet.addf32("farZ", mFar);
	
	auto copyCamera = ResourceManager::createCamera(paramSet);
	return copyCamera;
	
}

f32 ls::Pinhole::spawnRay(ls_Param_In SamplerPtr sampler,
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
	rec->spwanRay = DifferentialRay(ray);

	return f32();
}

void ls::Pinhole::sample(
	ls_Param_In SamplerPtr sampler,
	ls_Param_In const IntersectionRecord & refIts, 
	ls_Param_Out CameraSample* cameraSample,
	ls_Param_Out CameraSampleRecord * rec) const
{
	


	Point3 cameraPoint = mC2W(Point3(0.f, 0.f, 0.f));
	auto dir = Vec3(refIts.position - cameraPoint);
	auto dist = dir.length();
	dir /= dist;

	//相机空间的 Ray
	Ray cameraRay = mW2C(Ray(cameraPoint, dir));
	
	auto rasterPos = cameraRay(1.f / cameraRay.dir.z);

	rasterPos = mC2R(rasterPos);

	auto screenPos = Point2(rasterPos.x * mFilm->getWidth(), rasterPos.y * mFilm->getHeight());

	// 保存 新的 像素 坐标
	if (cameraSample)
		cameraSample->pos = screenPos;

	if (screenPos.x < 0.f ||
		screenPos.x >= mFilm->getWidth() ||
		screenPos.y < 0.f ||
		screenPos.y >= mFilm->getHeight())
	{
		rec->we = 0.f;
		rec->pdfD = 0.f;
		rec->pdfA = 1.f;
		return;
	}



	f32 cosTheta2 = cameraRay.dir.z * cameraRay.dir.z;
	rec->samplePosition = cameraPoint;
	rec->sampleDirection = dir;
	rec->we = 1.f / (mArea * cosTheta2 * cosTheta2);

	/*
		pdfD 为 采样方向的 pdf 
		该方向 为 cameraPoint 与 refIts.position 的连线 
		不需要考虑在 焦平面 的采样
		由于 不存在 lens , 所以 采样得到 cameraPoint 的 pdfA = 1
		由于其中一个点 （cameraPoint）为采样得到 （在这里是固定的）
		所以需要将 采样该点的 pdf （基于 面积 ）转换为 基于立体角
		
		由于是根据 refIts 采样，所以距离不是 焦平面的距离 
		而是  cameraPoint 与 refIts.position 的距离
	*/
	rec->pdfA = 1.f;
	rec->pdfD = RenderLib::pdfA2W(1.f, dist, std::fabs(cameraRay.dir.z));

}

void ls::Pinhole::sample(ls_Param_In SamplerPtr sampler,
	ls_Param_Out CameraSampleRecord * rec) const
{
	auto uv = sampler->next2D();


	Point3 pRaster = Point3(uv.x, uv.y, 1.f);
	Point3 pCamera = mR2C(pRaster);

	//相机空间的光线
	Ray cameraRay = Ray(Point3(0, 0, 0), normalize(Vec3(pCamera)));


	auto cosTheta = std::fabs(cameraRay.dir.z);
	auto cosTheta2 = cosTheta * cosTheta;
	//没有 lens pdfA = 1.f

	/*
		pdfD 方向的概率密度 = lens采样 PdfA (1.f) * 生成方向 Pdf
		生成方向 是通过与 焦平面 上 采样 得到的点 直接相连得到 pdf = 1 / 焦平面大小

		由于 两者 为 基于面积 ，需要转换成 基于立体角
	*/

	auto pdfA = 1.f;
	auto pdfD = RenderLib::pdfA2W(pdfA * 1.f / mArea, 1 / cosTheta, cosTheta);


	rec->samplePosition = mC2W(Point3(0.f, 0.f, 0.f));
	rec->sampleDirection = mC2W(cameraRay.dir);
	rec->we = 1.f / (mArea * 1.f * cosTheta2 * cosTheta2);
	rec->pdfA = pdfA;
	rec->pdfD = pdfD;
}

void ls::Pinhole::sample(ls_Param_In SamplerPtr sampler,
	ls_Param_In const CameraSample & sample, 
	ls_Param_Out CameraSampleRecord * rec) const
{
	

	auto uv = sample.pos; uv.x /= mFilm->getWidth(); uv.y /= mFilm->getHeight();

	
	Point3 pRaster = Point3(uv.x,uv.y,1.f);
	Point3 pCamera = mR2C(pRaster);

	//相机空间的光线
	Ray cameraRay = Ray(Point3(0, 0, 0), normalize(Vec3(pCamera)));

	
	auto cosTheta = std::fabs(cameraRay.dir.z);
	auto cosTheta2 = cosTheta * cosTheta;
	//没有 lens pdfA = 1.f
	
	/* 
		pdfD 方向的概率密度 = lens采样 PdfA (1.f) * 生成方向 Pdf
		生成方向 是通过与 焦平面 上 采样 得到的点 直接相连得到 pdf = 1 / 焦平面大小

		由于 两者 为 基于面积 ，需要转换成 基于立体角
	*/

	auto pdfA = 1.f;
	auto pdfD = RenderLib::pdfA2W(pdfA * 1.f / mArea, 1 / cosTheta, cosTheta);


	rec->samplePosition = mC2W(Point3(0.f, 0.f, 0.f));
	rec->sampleDirection = mC2W(cameraRay.dir);
	rec->n = mC2W(Normal(0.f, 0.f, 1.f));
	rec->we = 1.f / (mArea * 1.f * cosTheta * cosTheta);
	rec->pdfA = pdfA;
	rec->pdfD = pdfD;



}

f32 ls::Pinhole::pdf(ls_Param_In const CameraSampleRecord * rec) const
{
	Unimplement
	return f32();
}

ls::Vec3 ls::Pinhole::look() const
{
	return mC2W(Vec3(0.f,0.f,1.f));
}

void ls::Pinhole::commit() 
{
	if (!mFilm)
		ls_AssertMsg(false, "Lack Film in Pinhole");

	mFilm->commit();
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

	mC2R = C2R;
	mR2C = C2R.inverse();

	auto pMin = mR2C(Point3(0, 0, 0));
	auto pMax = mR2C(Point3(1, 1, 0));
	pMin.x /= pMin.z;
	pMin.y /= pMin.z;
	pMax.x /= pMax.z;
	pMax.y /= pMax.z;
	mArea = std::fabs((pMax.x - pMin.x) * (pMax.y - pMin.y));


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

std::string ls::Pinhole::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Camera: " << "Pinhole" << std::endl;
	oss << "ShutterStart: " << mShutterStart << std::endl;
	oss << "ShutterEnd: " << mShutterEnd << std::endl;
	oss << "Fov: " << mFov << std::endl;
	oss << "Near: " << mNear << std::endl;
	oss << "Far: " << mFar << std::endl;
	
	oss << "Film:" << std::endl;
	oss << mFilm->strOut();

	oss << "World:" << std::endl;
	oss << mW2C.toString() << std::endl;
	
	oss << ls_Separator << std::endl;
	return oss.str();
}

ls::Pinhole::Pinhole(ParamSet & paramSet)
{
	mFov = paramSet.queryf32("fov");
	mC2W = paramSet.queryTransform("c2w");
	mNear = paramSet.queryf32("nearZ", 1e-2);
	mFar = paramSet.queryf32("farZ", 1e4);
	mW2C = mC2W.inverse();
	mShutterStart = 0;
	mShutterEnd = 0;
}
