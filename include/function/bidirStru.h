#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<math/transform.h>
#include<math/frame.h>
#include<record/record.h>
#include<scatter/scatter.h>


//This file is for Bidirection Render Algorithm
namespace ls
{		
	enum PathVertexType
	{
		EPathVertex_Surface,
		EPathVertex_Medium,
		EPathVertex_Camera,
		EPathVertex_Light,
		EPathVertex_Invalid
	};
	class PathVertex
	{
		

	public:
		PathVertex() { mVertexType = EPathVertex_Invalid; }
		PathVertex(const PathVertex& v) 
		{
			transmode = v.transmode;
			position = v.position;
			ns = v.ns;
			wi = v.wi;
			wo = v.wo;
			pdfType = v.pdfType;
			pdfForward = v.pdfForward;
			pdfReverse = v.pdfReverse;

			next = v.next;
			pre = v.pre;
			mVertexType = v.mVertexType;
		
			switch (mVertexType)
			{
			case ls::EPathVertex_Surface:
				pathVertexRecord.surfaceSampleRecord = v.pathVertexRecord.surfaceSampleRecord;
				break;
			case ls::EPathVertex_Medium:
				pathVertexRecord.mediumSampleRecord = v.pathVertexRecord.mediumSampleRecord;
				break;
			case ls::EPathVertex_Camera:
				pathVertexRecord.cameraSampleRecord = v.pathVertexRecord.cameraSampleRecord;
				break;
			case ls::EPathVertex_Light:
				pathVertexRecord.lightSampleRecord = v.pathVertexRecord.lightSampleRecord;
				break;
			case ls::EPathVertex_Invalid:
				break;
			default:
				break;
			}
		}
		PathVertex& operator=(const PathVertex& v)
		{
			transmode = v.transmode;
			position = v.position;
			ns = v.ns;
			wi = v.wi;
			wo = v.wo;
			pdfType = v.pdfType;
			pdfForward = v.pdfForward;
			pdfReverse = v.pdfReverse;

			next = v.next;
			pre = v.pre;
			mVertexType = v.mVertexType;

			switch (mVertexType)
			{
			case ls::EPathVertex_Surface:
				pathVertexRecord.surfaceSampleRecord = v.pathVertexRecord.surfaceSampleRecord;
				break;
			case ls::EPathVertex_Medium:
				pathVertexRecord.mediumSampleRecord = v.pathVertexRecord.mediumSampleRecord;
				break;
			case ls::EPathVertex_Camera:
				pathVertexRecord.cameraSampleRecord = v.pathVertexRecord.cameraSampleRecord;
				break;
			case ls::EPathVertex_Light:
				pathVertexRecord.lightSampleRecord = v.pathVertexRecord.lightSampleRecord;
				break;
			case ls::EPathVertex_Invalid:
				break;
			default:
				break;
			}

			return *this;
		}
		

		~PathVertex(){}

		union PathVertexRecord
		{
			PathVertexRecord() {}
			~PathVertexRecord() {}

			LightSampleRecord	lightSampleRecord;
			CameraSampleRecord	cameraSampleRecord;
			SurfaceSampleRecord	surfaceSampleRecord;
			MediumSampleRecord	mediumSampleRecord;
		}pathVertexRecord;


		TransportMode		transmode;
		Point3				position;
		Normal				ns;
		Vec3				wi;
		Vec3				wo;

		ScatteringFlag		pdfType;

		/*
			pdfForward 代表根据实际路径生成方向得带该顶点的概率密度 （基于面积的PDF）

			pdfRerverse 则正好相反，代表 根据反方向生成该顶点的概率密度
		
		*/
		f32					pdfForward;
		f32					pdfReverse;

		/*
			该顶点的方向PDF

			Wi为采样方向
			Wo为给定方向

			Radiance 
			wo 

		*/
		f32					pdfWi;

		f32					getImportancePdf();
		f32					getRadiancePdf();

		PathVertexType		getVertexType() { return mVertexType; }
		bool				isValid() { return mVertexType != EPathVertex_Invalid; }

		void				getDirection(
			ls_Param_In TransportMode mode,
			ls_Param_Out Vec3* wi,
			ls_Param_Out Vec3* wo);


		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In SamplerPtr sampler,
			ls_Param_In LightPtr light,
			ls_Param_Out LightSampleRecord* lsr = nullptr);

		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In SamplerPtr sampler,
			ls_Param_In const CameraSample& cameraSample,
			ls_Param_In CameraPtr camera,
			ls_Param_Out CameraSampleRecord* csr = nullptr);

		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In ScatteringFunctionPtr bsdf,
			ls_Param_In SurfaceSampleRecord surfaceSampleRecord);

		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In ScatteringFunctionPtr medium,
			ls_Param_In MediumSampleRecord mediumSampleRecord);



		/*
			路径生成顺序:
			------------------->
			pre - > current -> next ( wo -> wi pdfReserve)
			  
		*/
		PathVertex*				next = nullptr;
		PathVertex*				pre = nullptr;

	private:
		PathVertexType		mVertexType;
		
	};

	class Path
	{
	public:

		auto size() const { return vertices.size(); }

		void addVertex(const PathVertex& vertex)
		{	
			/*
				顶点的添加顺序和生成顺序相同，与能量传播方向相反

				pre ---wi----> cur  
			*/
			vertices.push_back(vertex);
			if (vertices.size() > 1)
			{
				PathVertex& pre = vertices[vertices.size() - 2];
				PathVertex& cur = vertices[vertices.size() - 1];

				pre.next = &cur;
				cur.pre = &pre;

				

			}
		}

		PathVertex& operator[](s32 i) 
		{
			return vertices[i];
		}

		const PathVertex& operator[](s32 i) const
		{
			return vertices[i];
		}

		void clear()
		{
			vertices.clear();
		}


		static Path createPathFromCamera(
			ls_Param_In SamplerPtr sampler,
			ls_Param_In const CameraSample& cameraSample,
			ls_Param_In const CameraPtr camera);

		static Path createPathFromLight(
			ls_Param_In SamplerPtr sampler,
			ls_Param_In ScenePtr scene);

	private:
		std::vector<PathVertex> vertices;
	};
}