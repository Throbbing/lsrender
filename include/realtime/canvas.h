#pragma once
#include<config/config.h>
#include<config/common.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<function/log.h>
#include<array>




namespace ls
{
	/*
		实时画布接口
		
		实时画布只能在实时渲染线程中使用
		
	*/

	/*
		Canvas 中操作访问模式
		
		ECanvasOpAccess_CPU:
		先更新 CPU 资源，然后提交至 GPU

		ECanvasOpAccess_GPU:
		直接在 GPU 端更新资源，可能会导致资源不同步
	*/
	enum CanvasOpAccessMode
	{
		ECanvasOpAccess_CPU = 01,
		ECanvasOpAccess_GPU
	};

	class Canvas
	{
	public:
		Canvas() {}

		/*
			绘制 Quad
			
			[xStart,xEnd) * [yStart,yEnd)
			
		*/
		virtual void drawQuad(
			s32 xStart,s32 xEnd,
			s32 yStart,s32 yEnd,
			void* data,
			CanvasOpAccessMode accessMode) = 0;


		virtual void* getHardwareResource() = 0;


		virtual s32 getWidth() = 0;
		virtual s32 getHeight() = 0;
		virtual s32 getSlice() = 0;

		virtual s32 getDimension() = 0;
	protected:
	};

	/*
		创建不同维度不同数据类型的 Canvas 模板
	*/
	template<s32 Dim,typename Type>
	class CanvasX :public Canvas
	{
	public:
		CanvasX(const std::array<s32,Dim>& res,Type* data = nullptr):mResolution(res)
		{
			ls_Assert(Dim > 0 && Dim <= 3);
			
			s32 R = 1;
			for (auto p : mResolution)
				R *= p;
			
			mData = new Type[R];
			if (data)
			{
				s32 totalSize = 1;
				for (auto& p : mResolution)
					totalSize *= p;
				totalSize *= sizeof(Type);
				
				std::memcpy(mData, data, totalSize);
			}
		}

		virtual  ~CanvasX()
		{
			delete mData;
		}

		virtual s32 getWidth() override final
		{
			return mResolution[0];
		}
		virtual s32 getHeight() override final
		{
			if (Dim >= 2)
				return mResolution[1];
			else
				return -1;
		}
		virtual s32 getSlice() override final
		{
			if (Dim == 3)
				return mResolution[2];
			else
				return -1;
		}

		virtual s32 getDimension() override final
		{
			return Dim;
		}
		
	protected:
		Type* mData;
		std::array<s32,Dim> mResolution;
		
	};
	

	

	
}