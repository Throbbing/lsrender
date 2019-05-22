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
		
		//@samplePosition				��Դ�ϵĲ�����
		Point			samplePosition;
		//@sampleDirection				��Դ�������� �ӹ�Դ���򷢳�
		Vec3			sampleDirection;

		//@le							��Դ�����õ���radiance W/(sr* m^2)
		ls::Spectrum	le;


		//@pdfPos						��Դ�������pdf
		f32				pdfPos;
		//@pdfDir						��Դ���������pdf
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


		//@wi					������ߣ���������  
		//Mode == Radiance		ָ���Դ
		//Mode == Importance	ָ�����
		Vec3				wi;

		//@wo					�������
		//Mode == Radiance      ָ�����
		//Mode == Importance    ָ���Դ
		Vec3				wo;

		//@scatterFlag			scatteringFunction ��������
		u32					scatterFlag;

#if 0
		//@sampledRayFlag		�����õ�����
		//Note:					��ЩBSDF������Glass�������������ְ�������
		//						�ñ�ǩ�����ж� ������BSDF�ķ����������仹�Ƿ���
		//EScattering_Reflection   ����
		//EScattering_Transmission ����
		u32					sampledFlag;
#endif

		//@transportMode       ��������
		//Radiance			   �������ʼ����
		//Importance           �ӹ�Դ��ʼ����
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