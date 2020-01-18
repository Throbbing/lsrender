#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/module.h>
#include<spectrum/spectrum.h>
namespace ls
{	
	enum ScatteringFlag
	{
		//Diffuse
		EScattering_D				= 0b000'000'001,
		//Specular
		EScattering_S				= 0b000'000'010,
		//Glossy
		EScattering_G				= 0b100'000'000,
		EMeasure_SolidAngle			= 0b000'000'100, 
		EMeasure_Area				= 0b000'001'000,
		EScattering_Transmission	= 0b000'010'000,
		EScattering_Reflection		= 0b000'100'000,

		EScattering_Surface			= 0b001'000'000,
		EScattering_Medium			= 0b010'000'000,

		EScattering_Dummy			= 0b100'000'000
		
	};



	enum TransportMode
	{
		ETransport_Radiance			=0b000'000'001,
		ETransport_Importance		=0b000'000'010
	};


	

	/*
		   (100)      (000)        (010)    (000)      (001)  (000)    
		{Radiance, Importance} x {Forward, Reverse} x {Solid, Area}
	*/
#define PDFWrap_Radiance		0b100
#define PDFWrap_Importance		0b000
#define PDFWrap_Forward			0b010
#define PDFWrap_Reverse			0b000
#define PDFWrap_Solid			0b001
#define PDFWrap_Area			0b000

	struct PDFWrap
	{
		f32 get(s32 type) const { return pdfs[type]; }
		f32& set(s32 type) { return std::ref(pdfs[type]); }

		f32 pdfs[8];
	};


	class ScatteringFunction:public Module
	{

		friend Scene;
	public:
		ScatteringFunction(s32 flag,
			const std::string& id = "scatteringFunction"):Module(id) { mSFlag = flag; }
		virtual ~ScatteringFunction() {}

		virtual s32 scatteringFlag() { return mSFlag; }

		virtual bool isDelta() = 0;

		//采样Scatter 方向
		//@sampler			[in]		采样器
		//@rec				[out]		采样记录器 
		//Frame:						Local
		//Note: 
		//sample 函数填充ScatteringRecord中的 wi pdf scatterFlag sampledValue
		//并根据transportmode 填充 pdfRadiance 或 pdfImportance
		virtual void sample(ls_Param_In Sampler* sampler,
			ls_Param_In ls_Param_Out ScatteringRecord* rec) = 0;

#if 0
		virtual void sample(ls_Param_In Sampler* sampler,
			ls_Param_In const Record* refRec,
			ls_Param_Out Record* rec) = 0; 
#endif

		virtual f32 pdf(ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wo) = 0;
		virtual ls::Spectrum f(ls_Param_In const Vec3& wi,
			ls_Param_In const Vec3& wo) = 0;

	protected:
		s32			mSFlag;
	};


	class DummyScatter :public ScatteringFunction
	{
	public:
		DummyScatter(s32 flag = EScattering_Dummy):ScatteringFunction(EScattering_Dummy) {}

		bool isDelta() override { return false; }
		virtual void sample(ls_Param_In Sampler* sampler,
			ls_Param_In ls_Param_Out ScatteringRecord* rec)	override;
		
		virtual f32 pdf(ls_Param_In const Vec3& wi, ls_Param_In const Vec3& wo) override;
		virtual ls::Spectrum f(ls_Param_In const Vec3& wi, ls_Param_In const Vec3& wo) override;

		virtual std::string strOut() const override
		{
			return "DummyScatter";
		}
		virtual void commit() override
		{

		}

	};
}