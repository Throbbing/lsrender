#pragma once
#include<config\config.h>
#include<config\declaration.h>
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
		Point			samplePosition;
		Vec3			sampleDirection;
		ls::Spectrum	le;
		f32				pdfA;
		f32				pdfW;
		Light*			light = nullptr;
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
		Point		position;
		Normal		normal;

		Vec3		wi;
		Vec3		wo;
		ls::Spectrum	sampleValue;
		f32				pdfRadiance;
		f32				pdfImportance;
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
		ScatteringFunction*		bsdf = nullptr;
	};
	
}