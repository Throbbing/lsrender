#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<spectrum/spectrum.h>
namespace ls
{	
	enum ScatteringFlag
	{
		EScattering_D = 0b01,
		EScattering_S = 0b001,
		EMeasure_SolidAngle = 0b000'1,
		EMeasure_Area = 0b000'01,
		EScattering_Transmission = 0b000'001,
		EScattering_Reflection = 0b000'000'01,

		EScattering_Surface = 0b000'000'001,
		EScattering_Medium = 0b000'000'000'1,
		
	};
	class ScatteringFunction
	{

		friend Scene;
	public:
		ScatteringFunction(s32 flag) { mSFlag = flag; }
		virtual ~ScatteringFunction() {}

		virtual s32 scatteringFlag() { return mSFlag; }

		virtual bool isDelta() = 0;

		
		
		virtual void sample(ls_Param_In Sampler* sampler,
			ls_Param_Out ScatteringRecord* rec) = 0;

#if 0
		virtual void sample(ls_Param_In Sampler* sampler,
			ls_Param_In const Record* refRec,
			ls_Param_Out Record* rec) = 0; 
#endif
		virtual f32 pdf(ls_Param_In const ScatteringRecord* refRec) = 0;
		virtual ls::Spectrum f(ls_Param_In const ScatteringRecord* refRec) = 0;

	protected:
		s32			mSFlag;
	};
}