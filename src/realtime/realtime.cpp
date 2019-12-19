#include<realtime/realtime.h>
#include<function/stru.h>
void ls::RealtimeRenderer::addPointThreadSafe(const Point3 & p, const Vec3 & color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addPoint(p, color);
}

void ls::RealtimeRenderer::addPointThreadSafe(const Point2 & p, const Vec3 & color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addPoint(p);
}

void ls::RealtimeRenderer::addLineThreadSafe(const Point3 & p0, const Point3 & p1, const Vec3 & color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addLine(p0, p1, color);
}

void ls::RealtimeRenderer::addLineThreadSafe(const Point2 & p0, const Point2 & p1, const Vec3 & color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addLine(p0, p1, color);
}

void ls::RealtimeRenderer::addTriangleThreadSafe(const Point3 & p0, const Point3 & p1, const Point3 & p2, const Vec3 & color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addTriangle(p0, p1, p2, color);
}

void ls::RealtimeRenderer::addTriangleThreadSafe(const Point2 & p0, const Point2 & p1, const Point2 & p2,const Vec3& color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addTriangle(p0, p1, p2, color);
}

void ls::RealtimeRenderer::addQuadTexThreadSafe(const Point2 & minP, const Point2 & maxP, u32 width, u32 height, Vec4 * data)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addQuadTex(minP, maxP, width, height, data);

}

void ls::RealtimeRenderer::addAABBThreadSafe(const Point3 & minP, const Point3 & maxP, const Vec3 & color)
{
	std::lock_guard<std::mutex> lck(mMutex);
	addAABB(minP, maxP, color);
}

void ls::RealtimeRenderer::newFrame(f32 dt)
{
	static f32 t = 0.f;
	t += dt;
	/*
		需要锁住渲染线程，提取新的 渲染命令
	*/
	if (mSwitchRenderCommandBuffer)
	{
		std::lock_guard<std::mutex> lg(mMutex);

		/*
			保留长期渲染命令
		*/
		auto& lastQueue = mRenderCommandBuffer[mRenderCommandBufferIndex];
		auto& newQueue = mRenderCommandBuffer[1 - mRenderCommandBufferIndex];
		while (!lastQueue.empty())
		{
			auto command = lastQueue.back();
			lastQueue.pop_back();
			if (!command->once())
				newQueue.push_front(command);
		}

		mRenderCommandBuffer[mRenderCommandBufferIndex].clear();
		((LinearAllocator*)(mLinearAllocator[mRenderCommandBufferIndex]))->Reset();
		mRenderCommandBufferIndex = 1 - mRenderCommandBufferIndex;
		
		
		mSwitchRenderCommandBuffer = false;
	}
}

void ls::RealtimeRenderer::render()
{
	auto& commands = mRenderCommandBuffer[mRenderCommandBufferIndex];

	std::deque<RealtimeRenderCommandPtr> temp;

	while (!commands.empty())
	{
		auto command = commands.front();
		commands.pop_front();
		
		auto desc = command->str();
		
//		std::cout << desc << std::endl;
		command->run(Thread::getThreadID());

		if (!command->once())
			temp.push_back(command);
	}
	for (auto p : temp)
		commands.push_back(p);


}

void ls::RealtimeRenderer::addPoint(const Point3 & p, const Vec3 & color)
{
	mHasChanged = true;
	Point pp;
	pp.p = p;
	pp.color = color;
	mPoint3DData.push_back(pp);
}

void ls::RealtimeRenderer::addPoint(const Point2 & p, const Vec3 & color)
{
	mHasChanged = true;
	Point pp;
	pp.p = Point3(p.x,p.y,1);
	pp.color = color;
	mPoint2DData.push_back(pp);
}

void ls::RealtimeRenderer::addLine(const Point3 & p0, const Point3 & p1, const Vec3 & color)
{
	mHasChanged = true;
	Line ll;
	ll.p0 = p0;
	ll.p1 = p1;
	ll.color = color;
	mLine3DData.push_back(ll);
}

void ls::RealtimeRenderer::addLine(const Point2 & p0, const Point2 & p1, const Vec3 & color)
{
	mHasChanged = true;
	Line ll;
	ll.p0 = Point3(p0.x,p0.y,1.f);
	ll.p1 = Point3(p1.x,p1.y,1.f);
	ll.color = color;
	mLine2DData.push_back(ll);
}

void ls::RealtimeRenderer::addTriangle(const Point3 & p0, const Point3 & p1, const Point3 & p2, const Vec3 & color)
{
	mHasChanged = true;
	Triangle tt;
	tt.p0 = p0;
	tt.p1 = p1;
	tt.p2 = p2;
	tt.color = color;
	mTri3DData.push_back(tt);
}

void ls::RealtimeRenderer::addTriangle(const Point2 & p0, const Point2 & p1, const Point2 & p2, const Vec3 & color)
{
	mHasChanged = true;
	Triangle tt;
	tt.p0 = Point3(p0.x,p0.y,1.f);
	tt.p1 = Point3(p1.x,p1.y,1.f);
	tt.p2 = Point3(p2.x,p2.y,1.f);
	tt.color = color;
	mTri2DData.push_back(tt);
}

void ls::RealtimeRenderer::addQuadTex(const Point2 & minP, const Point2 & maxP, u32 width, u32 height, Vec4 * data)
{
	mHasChanged = true;
	QuadTex quad;
	quad.minP = minP;
	quad.maxP = maxP;
	quad.width = width;
	quad.height = height;

	

}

void ls::RealtimeRenderer::addAABB(const Point3 & minP, const Point3 & maxP, const Vec3 & color)
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

