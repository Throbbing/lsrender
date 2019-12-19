#pragma once
#include<config/config.h>
#include<config/common.h>
#include<3rd/LinearAllocator.h>
#include<3rd/StackAllocator.h>
#include<realtime/renderCommand.h>
#include<math/vector.h>
#include<function/log.h>
#include<thread>
#include<mutex>
#include<queue>
namespace ls
{
#define ls_UpdateRenderCommand_Interval 1.f

	class RealtimeRenderer
	{
	public:
		struct Point
		{
			Point3 p;
			Vec3   color;
		};
		struct Line
		{
			Point3 p0;
			Point3 p1;
			Vec3   color;
		};
		struct Triangle
		{
			Point3 p0, p1, p2;
			Vec3 color;
		};
		struct QuadTex
		{
			Point2 minP, maxP;
			u32 width, height;
			Vec4* data;
		};

		RealtimeRenderer(u64 renderCommandMemSize = (1 << 20))
		{
			mLinearAllocator[0] = new LinearAllocator(renderCommandMemSize); mLinearAllocator[0]->Init();
			mLinearAllocator[1] = new LinearAllocator(renderCommandMemSize); mLinearAllocator[1]->Init();
			mStackAllocator = new StackAllocator(renderCommandMemSize); mStackAllocator->Init();
		}
		virtual ~RealtimeRenderer() 
		{ 
			delete mLinearAllocator[0];
			delete mLinearAllocator[1];
			delete mStackAllocator;
		}

		

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





		virtual void resizeImmediate(s32 w,s32 h) = 0;
		

		virtual void newFrame(f32 dt);
		virtual void render();

		virtual void commit() = 0;


		/*
			渲染命令 入队列
		*/
		template<typename Desc,typename Func>
		static void enqueueRenderCommand(Func&& func)
		{
			ls_Assert(lsEnvironment::realtimeRenderer);

			// 当渲染线程在切换 渲染命令 buffer时
			// 暂停添加一切渲染命令至后备缓冲
			while (lsEnvironment::realtimeRenderer->mSwitchRenderCommandBuffer)
				;
			
			/*
				加锁来确保线程信息同步
			*/
			std::lock_guard<std::mutex> lg(lsEnvironment::realtimeRenderer->mMutex);
			auto renderCommand = (lsEnvironment::realtimeRenderer
				->mStackAllocator
				->Allocate<TRealtimeRenderCommand<Desc, Func>>(16, std::forward<Func>(func),false));
			
			// 渲染命令添加进渲染列表中
			lsEnvironment::realtimeRenderer->mRenderCommandBuffer[1 - lsEnvironment::realtimeRenderer->mRenderCommandBufferIndex].push_back(renderCommand);
		}

		template<typename Desc, typename Func>
		static void enqueueOnceRenderCommand(Func&& func)
		{
			ls_Assert(lsEnvironment::realtimeRenderer);

			// 当渲染线程在切换 渲染命令 buffer时
			// 暂停添加一切渲染命令至后备缓冲
			while (lsEnvironment::realtimeRenderer->mSwitchRenderCommandBuffer)
				;

			/*
				加锁来确保线程信息同步
			*/
			std::lock_guard<std::mutex> lg(lsEnvironment::realtimeRenderer->mMutex);
			auto renderCommand = (lsEnvironment::realtimeRenderer
					->mLinearAllocator[1 - lsEnvironment::realtimeRenderer->mRenderCommandBufferIndex]
					->Allocate<TRealtimeRenderCommand<Desc, Func>>(16,std::forward<Func>(func),true));

			// 渲染命令添加进渲染列表中
			lsEnvironment::realtimeRenderer->mRenderCommandBuffer[1 - lsEnvironment::realtimeRenderer->mRenderCommandBufferIndex].push_back(renderCommand);
		}

		static void triggleRenderCommnd()
		{
			ls_Assert(lsEnvironment::realtimeRenderer);

			lsEnvironment::realtimeRenderer->mSwitchRenderCommandBuffer = true;
		}



	protected:

		


		std::mutex					mMutex;

		/*
			当前渲染中使用的渲染命令缓存索引
		*/
		int							mRenderCommandBufferIndex = 0;

		std::atomic_bool			mSwitchRenderCommandBuffer = false;
		

		/*
			渲染命令 双缓冲
		*/
		std::deque<RealtimeRenderCommandPtr>  mRenderCommandBuffer[2];

		/*
			线性内存分配器
			用来 构造 一次 渲染命令
		*/
		Allocator*					mLinearAllocator[2];

		/*
			堆栈式内存分配器
			用来 构造 长期 渲染命令
		*/
		Allocator*					mStackAllocator = nullptr;

		std::vector<Point>			mPoint3DData;
		std::vector<Point>			mPoint2DData;
		std::vector<Line>			mLine3DData;
		std::vector<Line>			mLine2DData;
		std::vector<Triangle>		mTri3DData;
		std::vector<Triangle>		mTri2DData;
		std::vector<QuadTex>		mQuadTexData;

		bool						mHasChanged = false;
		
	};



}


/*
	宏命令
	添加渲染命令至后备列表
*/

struct AnonymousRenderCommandDescriptionName
{
	static const char* str() { return "AnonymousRenderCommandDescription"; }

};


#define ls_Enqueue_RenderCommand(Type) \
	struct Type##Name \
	{	\
		static const char* str() { return #Type;}	\
	};\
	ls::RealtimeRenderer::enqueueRenderCommand<Type##Name>

#define ls_Enqueue_AnonymousRenderCommand ls::RealtimeRenderer::enqueueRenderCommand<AnonymousRenderCommandDescriptionName>


#define ls_Enqueue_OnceRenderCommand(Type) \
	struct Type##OnceName \
	{	\
		static const char* str() { return #Type;}	\
	};\
	ls::RealtimeRenderer::enqueueOnceRenderCommand<Type##OnceName>

#define ls_Enqueue_AnonymousOnceRenderCommand ls::RealtimeRenderer::enqueueOnceRenderCommand<AnonymousRenderCommandDescriptionName>

/*
	
*/
#define ls_Trigger_RenderCommand ls::RealtimeRenderer::triggleRenderCommnd();