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


	class Distribution
	{
	public:
		Distribution(f32 alphaU,
			f32 alphaV,
			DistributionType type,
			bool sampleAll = true
		) 
		{
			mAlphaU = alphaU;
			mAlphaV = alphaV;
			mDistributionType = type;
			mSampleAllDistribution = sampleAll;
		}

		//����Wh
		//@sampler			[in]		������
		//@w				[in]		�������䷽��		
		//@wh				[out]       �����õ��İ�����
		//@pdfH				[out]		����pdfH�õ���
		//Frame:						Local
		//Note: 
		//���أ�D(wh)
		f32 sample(
			ls_Param_In SamplerPtr sampler,
			ls_Param_In const Vec3& w,
			ls_Param_Out Vec3* wh,
			ls_Param_Out f32* pdfH
		) const;

		//����D(wh)
		//wh = normalize(wi + wo)
		f32 D(
			ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wo) const;

		//����D(wh)
		f32 D(
			ls_Param_In const Vec3& wh
		) const;

		f32 G(
			ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wh
		);

		f32 G(ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wo,
			ls_Param_In const Vec3& wh);

		//����pdfH(w,wh)
		virtual f32 pdf(
			ls_Param_In const Vec3& w,
			ls_Param_In const Vec3& wh) const;



		bool isotropic() { return mAlphaU == mAlphaV; }

	protected:
		

		f32 mAlphaU;
		f32 mAlphaV;
		DistributionType	mDistributionType;
		bool	mSampleAllDistribution;
	};
}