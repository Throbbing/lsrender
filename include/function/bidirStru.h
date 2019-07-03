#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<function/stru.h>
#include<math/transform.h>
#include<math/frame.h>
#include<record/record.h>
#include<scatter/scatter.h>
#include<spectrum/spectrum.h>


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

	enum PathType
	{
		EPath_Camera,
		EPath_Light
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
			scatter = v.scatter;
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

		ScatteringFunctionPtr	scatter = nullptr;

		ScatteringFlag		pdfType;

		/*
			pdfForward 代表根据实际路径生成方向 (wi) 得到该顶点的概率密度 （基于面积的PDF）
			
			pdfRerverse 则正好相反，代表根据反方向 (wo) 生成该顶点的概率密度

		*/
		f32					pdfForward;
		f32					pdfReverse;

		/*
			该顶点的方向PDF

			Wi为采样方向
			Wo为给定方向

			Radiance 
			wo 指向 相机
			wi 指向 光源

			Importance
			wo 指向 光源
			wi 指向 相机
		*/
		f32					pdfWi;
		f32					pdfWo;


		/*
			路径生成到该顶点时的throughput
		*/
		Spectrum			throughput;
		
		/*
			Radiance Transport 中生成该顶点的PDF （基于面积）
		*/
		f32					getRadiancePdf() const;

		/*
			Importance Transport 中生成该顶点的PDF (基于面积)
		*/
		f32					getImportancePdf() const;


		/*
			计算 pdfForward 需要 pre 顶点有效
		*/
		bool				updatePdfForward();

		/*
			计算 pdfReverse 需要 next 顶点有效
		*/
		bool				updatePdfReverse();



		PathVertexType		getVertexType() const { return mVertexType; }

		bool				isValid() const { return mVertexType != EPathVertex_Invalid; }

		void				getDirection(
			ls_Param_In TransportMode mode,
			ls_Param_Out Vec3* wi,
			ls_Param_Out Vec3* wo);

		//从光源生成顶点
		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In SamplerPtr sampler,
			ls_Param_In LightPtr light,
			ls_Param_Out LightSampleRecord* lsr = nullptr);


		//从相机生成顶点
		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In SamplerPtr sampler,
			ls_Param_In const CameraSample& cameraSample,
			ls_Param_In CameraPtr camera,
			ls_Param_Out CameraSampleRecord* csr = nullptr);

		//在光源上 (面积光和环境光) 生成顶点
		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In LightPtr light,
			ls_Param_In const DifferentialRay& ray,
			ls_Param_In const IntersectionRecord& itsRecord,
			ls_Param_In const Spectrum& throughput
		);

		//在表面生成顶点
		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In ScatteringFunctionPtr bsdf,
			ls_Param_In const SurfaceSampleRecord& surfaceSampleRecord,
			ls_Param_In const Spectrum& throughput);

		//在介质中生成顶点
		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In ScatteringFunctionPtr medium,
			ls_Param_In const MediumSampleRecord& mediumSampleRecord,
			ls_Param_In const Spectrum& throughput);

		
		// 判断两个顶点是否可以相连
		static bool connectable(
			ls_Param_In const PathVertex& a,
			ls_Param_In const PathVertex& b);

		/*
			路径生成顺序:
			------------------->
			pre - > current -> next ( wo -> wi )
			  
		*/
		PathVertex*				next = nullptr;
		PathVertex*				pre = nullptr;

	private:
		PathVertexType		mVertexType;
		
	};

	class Path
	{
	public:
		Path(PathType type) :mPathType(type) {}

		auto getPathType() const { return mPathType; }
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

				// 更新相邻顶点的 基于面积的PDF
				pre.updatePdfReverse();
				cur.updatePdfForward();
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
			ls_Param_In ScenePtr scene,
			ls_Param_In const CameraSample& cameraSample,
			ls_Param_In const CameraPtr camera,
			ls_Param_In s32 maxDepth);

		static Path createPathFromLight(
			ls_Param_In SamplerPtr sampler,
			ls_Param_In ScenePtr scene,
			ls_Param_In s32 maxDepth);

		static void randomWalk(
			ls_Param_In ScenePtr scene,
			ls_Param_In SamplerPtr sampler,
			ls_Param_In const DifferentialRay& ray,
			ls_Param_In Spectrum throughput,
			ls_Param_In TransportMode transMode,
			ls_Param_In s32 maxDepth,
			ls_Param_In ls_Param_Out Path* path);

		

	private:
		std::vector<PathVertex> vertices;
		PathType				mPathType;
	};
}