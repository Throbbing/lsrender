#pragma once
#include<config\config.h>
#include<config\declaration.h>
#include<config/lsPtr.h>
#include<function\log.h>
#include<function\func.h>
#include<Math\math.h>
#include<Math\matrix.h>
#include<Spectrum\spectrum.h>
#include<function\stru.h>
namespace ls
{
// forward declaration


#if 0
#define CameraSamplePtrCast(p)		safePtrCast<Record,CameraSampleRecord>(p)
#define LightSamplePtrCast(p)		safePtrCast<Record,LightSampleRecord>(p)
#define MeshSamplePtrCast(p)		safePtrCast<Record,MeshSampleRecord>(p)
#define MediumSamplePtrCast(p)		safePtrCast<Record,MediumSampleRecord>(p)
#define SurfaceSamplePtrCast(p)		safePtrCast<Record,SurfaceSampleRecord>(p)
#define IntersectionPtrCast(p)		safePtrCast<Record,IntersectionRecord>(p)

#define CameraSampleSmartCast(p)	safeSmartPtrCast<Record,CameraSampleRecord>(p)
#define LightSampleSmartCast(p)		safeSmartPtrCast<Record,LightSampleRecord>(p)
#define MeshSampleSmartCast(p)		safeSmartPtrCast<Record,MeshSampleRecord>(p)
#define MediumSampleSmartCast(p)	safeSmartPtrCast<Record,MediumSampleRecord>(p)
#define SurfaceSampleSmartCast(p)	safeSmartPtrCast<Record,SurfaceSampleRecord>(p)
#define IntersectionSmartCast(p)	safeSmartPtrCast<Record,IntersectionRecord>(p)
#endif

	struct RTCRecord
	{
	public:
		s32 geomID = -1;
		s32 primID = -1;
		RTCRayHit   rayHit;
	};



	struct CameraSpwanRayRecord 
	{
	public:
		DifferentialRay			spwanRay;

	};

	struct CameraSampleRecord
	{
	public:

		// $ We = W^0_e * W^1_e $
		Point			samplePosition;
		Vec3			sampleDirection;
		ls::Spectrum	we;
		f32				pdfA;
		f32				pdfD;
		f32				time;
		Camera*			camera;
	};

	struct LightSampleRecord 
	{
	public:
		// $ Le = L^0_e * L^1_e $
		
		//@samplePosition				光源上的采样点
		Point			samplePosition;
		//@sampleDirection				光源采样方向 从光源方向发出
		Vec3			sampleDirection;

		//@le							光源采样得到的radiance W/(sr* m^2)
		ls::Spectrum	le;


		//@pdfPos						光源采样点的pdf
		f32				pdfPos;
		//@pdfDir						光源采样方向的pdf
		f32				pdfDir;

		//@pdfA							pdfPos * pdfDir
		f32				pdfA;
		//@pdfW							pdfPos * pdfDir
		f32				pdfW;


		LightPtr		light = nullptr;
		u32				mode;
	};

	struct MeshSampleRecord 
	{
	public:
		Point		samplePosition;
		Normal		surfaceNormal;
		Vec3		sampleDirection;

		
		f32			pdfA;
		f32			pdfW;

		Mesh*		mesh = nullptr;
	};

	struct ScatteringRecord 
	{
	public:
		Point				position;
		Normal				normal;


		//@wi					入射光线（采样方向）  
		//Mode == Radiance		指向光源
		//Mode == Importance	指向相机
		Vec3				wi;

		//@wo					出射光线
		//Mode == Radiance      指向相机
		//Mode == Importance    指向光源
		Vec3				wo;

		//@scatterFlag			scatteringFunction 采样类型
		u32					scatterFlag;

#if 0
		//@sampledRayFlag		采样得到类型
		//Note:					有些BSDF（例如Glass）即包含反射又包含折射
		//						该标签用来判断 采样后BSDF的方向，属于折射还是反射
		//EScattering_Reflection   反射
		//EScattering_Transmission 折射
		u32					sampledFlag;
#endif

		//@transportMode       传输类型
		//Radiance			   从相机开始传输
		//Importance           从光源开始传输
		u32					transportMode;

		ls::Spectrum		sampledValue;
		f32					pdfRadiance;
		f32					pdfImportance;
		f32					pdf;
	};



	struct MediumSampleRecord :public ScatteringRecord
	{
	public:
		Medium*		medium;

	};

	struct SurfaceSampleRecord :public ScatteringRecord
	{
	public:
		Mesh*		mesh;
	};



	struct IntersectionRecord 
	{
	public:
		Point		position;
		Normal		ng;
		Normal		ns;
		Vec2		uv;

		Vec3		dpdx;
		Vec3		dpdy;
		Vec3		dpdu;
		Vec3		dpdv;

		Vec3		dndx;
		Vec3		dndy;
		Vec3		dndu;
		Vec3		dndv;

		Light*					areaLight = nullptr;
		MaterialPtr				material = nullptr;
	};
	
}