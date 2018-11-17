#pragma once

#include<config/config.h>
#include<config/declaration.h>
#include<math/vector.h>
#include<math/math.h>
namespace ls
{
	class Ray
	{
	public:
		Ray() :tnear(lsMath::Epsilon), tfar(lsMath::Infinity), time(0.f) {}
		Ray(Point3 o, Vec3 d, s32 _depth = 0,f32 _time = 0.f, f32 near = lsMath::Epsilon, f32 far = lsMath::Infinity):
			ori(o),dir(d),depth(_depth),time(_time),tnear(near),tfar(far)
		{

		}
		virtual ~Ray(){}
		
		Point operator()(f32 t) const  { return ori + t*dir; }
		

		Point3	ori;
		Vec3	dir;

		mutable f32		tfar;
		mutable f32		tnear;
		mutable f32		time;
		mutable s32		depth;
		mutable bool	hasDifferentials;

	};

	class DifferentialRay :public Ray
	{
	public:
		DifferentialRay() {}
		DifferentialRay(Point3 o, Vec3 d,s32 _depth = 0, f32 _time = 0.f, f32 near = lsMath::Epsilon, f32 far = lsMath::Infinity) :
			Ray(o, d, _depth,_time, near, far) {}
		DifferentialRay(const Ray& r) :Ray(r) {}

		virtual ~DifferentialRay() {}
		Point3 oriX;
		Point3 oriY;
		Vec3   dirX;
		Vec3   dirY;
	};

	class AABB
	{
	public:
		AABB()
		{
			minP = Point3(lsMath::Infinity, lsMath::Infinity, lsMath::Infinity);
			maxP = Point3(lsMath::Epsilon, lsMath::Epsilon, lsMath::Epsilon);
		}
		AABB(const Point3& minp, const Point3& maxp) :
			minP(minp), maxP(maxp) {}

		f32 extent(s32 i) { return maxP[i] - minP[i]; }
		void unionAABB(const AABB& aabb);
		bool inside(const Point3& p);

		bool intersect(ls_Param_In const Ray& ray,
			ls_Param_Out f32* t = nullptr);



		Point3 minP;
		Point3 maxP;
	};
	

	
}