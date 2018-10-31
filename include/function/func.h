#pragma once


#include<function/log.h>
#include<config/config.h>
#include<config/declaration.h>
#include<math/vector.h>
#include<memory>
namespace ls
{
	template<typename Ori,typename Dest>
	Dest* safePtrCast(Ori* ptr)
	{
		ls_AssertMsg(typeid(*ptr) == typeid(Dest), "Unexpected cast error encountered ");
		return static_cast<Dest*>(ptr);
	}

	template<typename Ori,typename Dest>
	std::shared_ptr<Dest> safeSmartPtrCast(const std::shared_ptr<Ori>& ptr)
	{
		ls_AssertMsg(typeid(*ptr) == typeid(Dest), "Unexpected cast error encountered ");
		return std::static_pointer_cast<Dest>(ptr);
	}


	ls_ForceInline void clearMemory(void* p, u32 size)
	{
		std::memset(p, 0, size);
	}

	

	struct RenderLib
	{
	public:
		static f32 mis(s32 nf, f32 pf, s32 ng, f32 pg);
		static f32 mis(f32 pf, f32 pg);
		static f32 pdfW2A(f32 pdfW, f32 r, f32 cos);
		static f32 pdfA2W(f32 pdfA, f32 r, f32 cos);
		static bool visible(Scene* scene, Point p0, Point p1);
		static bool globalSample(ls_Param_In Scene* scene, ls_Param_In Sampler* sampler,
			ls_Param_Out MeshSampleRecord* meshRec);
	};
	

}