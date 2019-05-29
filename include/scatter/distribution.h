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

		//����Wh
		//@sampler			[in]		������
		//@w				[in]		�������䷽��		
		//@wh				[out]       �����õ��İ�����
		//@pdfH				[out]		����pdfH�õ���
		//Frame:						Local
		//Note: 
		//���أ�D(wh)
		virtual Spectrum sample(
			ls_Param_In SamplerPtr sampler,
			ls_Param_In const Vec3& w,
			ls_Param_Out Vec3* wh,
			ls_Param_Out f32* pdfH
		) const = 0;

		//����D(wh)
		//wh = normalize(wi + wo)
		virtual Spectrum D(
			ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wo) const;

		//����D(wh)
		virtual Spectrum D(
			ls_Param_In const Vec3& wh
		) const = 0;

		//����pdfH(wh)
		virtual f32 pdf(
			ls_Param_In const Vec3& wo,
			ls_Param_In const Vec3& wi) const;

		//����pdfH(wh)
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