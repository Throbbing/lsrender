#pragma once

#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
#include<spectrum/spectrum.h>
namespace ls
{
	class Light:public Module
	{
		friend Scene;
	public:
		Light(const std::string& id = "Light"):Module(id){}
		virtual ~Light() {}

		virtual bool		isDelta() = 0;
		virtual Spectrum	getPower() = 0;

		virtual void attachMesh(MeshPtr mesh) { mAttachedMesh = mesh; }
		virtual void detachMesh() { mAttachedMesh = nullptr; }


		//Sample
		//根据光源分布采样得到新的方向和位置
		//@sampler		[in]		采样器
		//@rec			[out]		记录器 保存采样得到的光线
		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_Out LightSampleRecord* rec) = 0;

		//Sample
		//根据已有碰撞点和光源分布得到新的方向和位置
		//@sampler		[in]		采样器
		//@refRec		[in]		表面碰撞记录器
		//@ref			[in]		记录器 保存采样得到的光线		
		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_In const IntersectionRecord* refRec,
			ls_Param_Out LightSampleRecord* rec) = 0;


		//Sample
		//光源在某个方向上发射出的能量
		//@ray			[in]		采样光线
		//				[return]	发射能量
		virtual ls::Spectrum sample(const Ray& ray) { return ls::Spectrum(0); }
		virtual f32			 pdf(const Ray& ray) { return 0.f; }

		//Sample
		//返回记录器中光线所需要的Pdf
		//@refRef		[in]		记录器
		//				[return]	pdf
		virtual f32 pdf(ls_Param_In const LightSampleRecord* refRec) = 0;


		virtual void commit() = 0;
		

	protected:
		MeshPtr					mAttachedMesh = nullptr;
	};
}