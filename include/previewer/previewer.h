#pragma once
#include<config/config.h>
#include<config/common.h>
#include<math/vector.h>
#include<thread>
#include<mutex>
namespace ls
{
	class Previewer
	{
	public:
		struct PrePoint
		{
			Point3 p;
			Vec3   color;
		};
		struct PreLine
		{
			Point3 p0;
			Point3 p1;
			Vec3   color;
		};
		struct PreTri
		{
			Point3 p0, p1, p2;
			Vec3 color;
		};
		struct PreQuadTex
		{
			Point2 minP, maxP;
			u32 width, height;
			Vec4* data;
		};

		Previewer() {}
		virtual ~Previewer() {}

		virtual void addPoint(const Point3& p,
			const Vec3& color = Vec3(1,0,0));
		virtual void addPoint(const Point2& p, 
			const Vec3& color = Vec3(1, 0, 0));
		virtual void addLine(const Point3& p0,
			const Point3& p1, 
			const Vec3& color = Vec3(1, 0, 0));
		virtual void addLine(const Point2& p0,
			const Point2& p1,
			const Vec3& color = Vec3(1, 0, 0));
		virtual void addTriangle(const Point3& p0,
			const Point3& p1, const Point3& p2,
			const Vec3& color = Vec3(1, 0, 0));
		virtual void addTriangle(const Point2& p0,
			const Point2& p1, const Point2& p2,
			const Vec3& color = Vec3(1, 0, 0));

		virtual void addQuadTex(const Point2& minP,
			const Point2& maxP,
			u32 width, u32 height,
			Vec4* data);

		virtual void addAABB(const Point3& minP,
			const Point3& maxP,
			const Vec3& color);


		virtual void addPointThreadSafe(const Point3& p,
			const Vec3& color = Vec3(1, 0, 0));
		virtual void addPointThreadSafe(const Point2& p,
			const Vec3& color = Vec3(1, 0, 0));
		virtual void addLineThreadSafe(const Point3& p0,
			const Point3& p1,
			const Vec3& color = Vec3(1, 0, 0));
		virtual void addLineThreadSafe(const Point2& p0,
			const Point2& p1,
			const Vec3& color = Vec3(1, 0, 0));
		virtual void addTriangleThreadSafe(const Point3& p0,
			const Point3& p1, const Point3& p2,
			const Vec3& color = Vec3(1, 0, 0));
		virtual void addTriangleThreadSafe(const Point2& p0,
			const Point2& p1, const Point2& p2,
			const Vec3& color = Vec3(1, 0, 0));

		virtual void addQuadTexThreadSafe(const Point2& minP,
			const Point2& maxP,
			u32 width, u32 height,
			Vec4* data);

		virtual void addAABBThreadSafe(const Point3& minP,
			const Point3& maxP,
			const Vec3& color);





		virtual void newFrame(f32 dt) = 0;
		virtual void render() = 0;

		virtual void commit() = 0;


	protected:
		std::mutex				mMutex;
		std::vector<PrePoint>	mPoint3DData;
		std::vector<PrePoint>	mPoint2DData;
		std::vector<PreLine>	mLine3DData;
		std::vector<PreLine>	mLine2DData;
		std::vector<PreTri>		mTri3DData;
		std::vector<PreTri>		mTri2DData;
		std::vector<PreQuadTex>	mQuadTexData;

		bool					mHasChanged = false;
		
	};
}
