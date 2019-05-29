#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
#include<config/common.h>


namespace ls
{
	enum DistributionType
	{
		EDistribution_Beckman,
		EDistribution_GGX
	};
	class Distribution :public Module
	{
	public:
		Distribution(const std::string id = "distribution") :Module(id) {}

		//采样Wh
		//@sampler			[in]		采样器
		//@w				[in]		入射或出射方向		
		//@wh				[out]       采样得到的半向量
		//@pdfH				[out]		采样pdfH得到的
		//Frame:						Local
		//Note: 
		//返回：D(wh)
		virtual Spectrum sample(
			ls_Param_In SamplerPtr sampler,
			ls_Param_In const Vec3& w,
			ls_Param_Out Vec3* wh,
			ls_Param_Out f32* pdfH
		) const = 0;

		//计算D(wh)
		//wh = normalize(wi + wo)
		virtual Spectrum D(
			ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wo) const;

		//计算D(wh)
		virtual Spectrum D(
			ls_Param_In const Vec3& wh
		) const = 0;

		//计算pdfH(wh)
		virtual f32 pdf(
			ls_Param_In const Vec3& wo,
			ls_Param_In const Vec3& wi) const;

		//计算pdfH(wh)
		virtual f32 pdf(
			ls_Param_In const Vec3& wh
		) const = 0;

		bool isotropic() { return mAlphaU == mAlphaV; }

	protected:
		f32 mAlphaU;
		f32 mAlphaV;
		DistributionType	mDistributionType;
	};
}