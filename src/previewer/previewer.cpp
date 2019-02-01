#include<previewer/previewer.h>
#include<function/stru.h>
void ls::Previewer::addPointThreadSafe(const Point3 & p, const Vec3 & color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addPoint(p, color);
}

void ls::Previewer::addPointThreadSafe(const Point2 & p, const Vec3 & color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addPoint(p);
}

void ls::Previewer::addLineThreadSafe(const Point3 & p0, const Point3 & p1, const Vec3 & color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addLine(p0, p1, color);
}

void ls::Previewer::addLineThreadSafe(const Point2 & p0, const Point2 & p1, const Vec3 & color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addLine(p0, p1, color);
}

void ls::Previewer::addTriangleThreadSafe(const Point3 & p0, const Point3 & p1, const Point3 & p2, const Vec3 & color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addTriangle(p0, p1, p2, color);
}

void ls::Previewer::addTriangleThreadSafe(const Point2 & p0, const Point2 & p1, const Point2 & p2,const Vec3& color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addTriangle(p0, p1, p2, color);
}

void ls::Previewer::addQuadTexThreadSafe(const Point2 & minP, const Point2 & maxP, u32 width, u32 height, Vec4 * data)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addQuadTex(minP, maxP, width, height, data);

}

void ls::Previewer::addAABBThreadSafe(const Point3 & minP, const Point3 & maxP, const Vec3 & color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addAABB(minP, maxP, color);
}

void ls::Previewer::addPoint(const Point3 & p, const Vec3 & color)
{
	mHasChanged = true;
	PrePoint pp;
	pp.p = p;
	pp.color = color;
	mPoint3DData.push_back(pp);
}

void ls::Previewer::addPoint(const Point2 & p, const Vec3 & color)
{
	mHasChanged = true;
	PrePoint pp;
	pp.p = Point3(p.x,p.y,1);
	pp.color = color;
	mPoint2DData.push_back(pp);
}

void ls::Previewer::addLine(const Point3 & p0, const Point3 & p1, const Vec3 & color)
{
	mHasChanged = true;
	PreLine ll;
	ll.p0 = p0;
	ll.p1 = p1;
	ll.color = color;
	mLine3DData.push_back(ll);
}

void ls::Previewer::addLine(const Point2 & p0, const Point2 & p1, const Vec3 & color)
{
	mHasChanged = true;
	PreLine ll;
	ll.p0 = Point3(p0.x,p0.y,1.f);
	ll.p1 = Point3(p1.x,p1.y,1.f);
	ll.color = color;
	mLine2DData.push_back(ll);
}

void ls::Previewer::addTriangle(const Point3 & p0, const Point3 & p1, const Point3 & p2, const Vec3 & color)
{
	mHasChanged = true;
	PreTri tt;
	tt.p0 = p0;
	tt.p1 = p1;
	tt.p2 = p2;
	tt.color = color;
	mTri3DData.push_back(tt);
}

void ls::Previewer::addTriangle(const Point2 & p0, const Point2 & p1, const Point2 & p2, const Vec3 & color)
{
	mHasChanged = true;
	PreTri tt;
	tt.p0 = Point3(p0.x,p0.y,1.f);
	tt.p1 = Point3(p1.x,p1.y,1.f);
	tt.p2 = Point3(p2.x,p2.y,1.f);
	tt.color = color;
	mTri2DData.push_back(tt);
}

void ls::Previewer::addQuadTex(const Point2 & minP, const Point2 & maxP, u32 width, u32 height, Vec4 * data)
{
	mHasChanged = true;
	PreQuadTex quad;
	quad.minP = minP;
	quad.maxP = maxP;
	quad.width = width;
	quad.height = height;

	

}

void ls::Previewer::addAABB(const Point3 & minP, const Point3 & maxP, const Vec3 & color)
{
	auto extent = maxP - minP;
	
	Point3 p0 = minP;
	Point3 p1 = minP + Point3(extent.x, 0, 0);
	Point3 p2 = minP + Point3(0, extent.y, 0);
	Point3 p3 = minP + Point3(extent.x, extent.y, 0);

	Point3 p4 = maxP;
	Point3 p5 = maxP - Point3(extent.x, 0, 0);
	Point3 p6 = maxP - Point3(0, extent.y, 0);
	Point3 p7 = maxP - Point3(extent.x, extent.y, 0);


	addLine(p0, p1, color);
	addLine(p0, p2, color);
	addLine(p3, p2, color);
	addLine(p3, p1, color);

	addLine(p0, p7, color);
	addLine(p2, p5, color);
	addLine(p3, p4, color);
	addLine(p1, p6, color);

	addLine(p7, p5, color);
	addLine(p7, p6, color);
	addLine(p4, p5, color);
	addLine(p4, p6, color);
}




