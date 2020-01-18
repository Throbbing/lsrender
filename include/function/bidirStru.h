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


	class BiDirPath;
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
			its = v.its;
			material = v.material;
			pdfType = v.pdfType;
			pdfForward = v.pdfForward;
			pdfReverse = v.pdfReverse;
			pdfWi = v.pdfWi;
			pdfWo = v.pdfWo;
			throughput = v.throughput;

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
			its = v.its;
			material = v.material;
			pdfType = v.pdfType;
			pdfForward = v.pdfForward;
			pdfReverse = v.pdfReverse;
			pdfWi = v.pdfWi;
			pdfWo = v.pdfWo;
			throughput = v.throughput;

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

//		ScatteringFunctionPtr	scatter = nullptr;
		MaterialPtr			material = nullptr;
		IntersectionRecord  its;
		ScatteringFlag		pdfType;

		/*
			pdfForward �������ʵ��·�����ɷ��� (wi) �õ��ö���ĸ����ܶ� �����������PDF��
			
			pdfRerverse �������෴��������ݷ����� (wo) ���ɸö���ĸ����ܶ�

		*/
		f32					pdfForward = 0.f;
		f32					pdfReverse = 0.f;

		/*
			�ö���ķ���PDF

			WiΪ��������
			WoΪ��������

			Radiance 
			wo ָ�� ���
			wi ָ�� ��Դ

			Importance
			wo ָ�� ��Դ
			wi ָ�� ���
		*/
		f32					pdfWi = 0.f;
		f32					pdfWo = 0.f;


		/*
			·�����ɵ��ö���ʱ��throughput
		*/
		Spectrum			throughput;
		
		/*
			Radiance Transport �����ɸö����PDF �����������
		*/
		f32					getRadiancePdf() const;

		/*
			Importance Transport �����ɸö����PDF (�������)
		*/
		f32					getImportancePdf() const;


		/*
			
		*/

		/*
			���� pdfForward ��Ҫ pre ������Ч
		*/
		bool				updatePdfForward(const BiDirPath& path);

		/*
			���� pdfReverse ��Ҫ next ������Ч
		*/
		bool				updatePdfReverse(const BiDirPath& path);

		/*
			��ȡ Intersection
		*/
		IntersectionRecord	getIntersection() const
		{
			// ֻ�� surface �ϵĶ�����ܻ�ȡ��ײ��Ϣ
			ls_Assert(mVertexType == EPathVertex_Surface);
			return its;
		}

		/*
			�жϸö����Ƿ����
		*/
		bool isConnectable() const;


		PathVertexType		getVertexType() const { return mVertexType; }

		bool				isValid() const { return mVertexType != EPathVertex_Invalid; }

		void				getDirection(
			ls_Param_In TransportMode mode,
			ls_Param_Out Vec3* wi,
			ls_Param_Out Vec3* wo);

		// �ӹ�Դ���ɶ���
		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In SamplerPtr sampler,
			ls_Param_In LightPtr light,
			ls_Param_In f32 selectLightPdf,
			ls_Param_Out LightSampleRecord* lsr = nullptr);

		// �Ӹ����Ĺ�Դ������ �� refPoint ���ɶ���
		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In f32 selectLightPdf,
			ls_Param_In const Point3& refP,
			ls_Param_In const LightSampleRecord& lsr
		);

		//��������ɶ���
		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In SamplerPtr sampler,
			ls_Param_In const CameraSample& cameraSample,
			ls_Param_In CameraPtr camera,
			ls_Param_Out CameraSampleRecord* csr = nullptr);

		//�ڹ�Դ�� (�����ͻ�����) ���ɶ���
		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In LightPtr light,
			ls_Param_In const DifferentialRay& ray,
			ls_Param_In const IntersectionRecord& itsRecord,
			ls_Param_In const Spectrum& throughput
		);

		//�ڱ������ɶ���
		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In const IntersectionRecord& its,
			ls_Param_In const SurfaceSampleRecord& surfaceSampleRecord,
			ls_Param_In const Spectrum& throughput);

		//�ڽ��������ɶ���
		static PathVertex createPathVertex(
			ls_Param_In TransportMode mode,
			ls_Param_In MaterialPtr material,
			ls_Param_In const MediumSampleRecord& mediumSampleRecord,
			ls_Param_In const Spectrum& throughput);

		
		// �ж����������Ƿ��������
		static bool connectable(
			ls_Param_In const PathVertex& a,
			ls_Param_In const PathVertex& b);

		// ������������ľ���
		static f32 distance(
			ls_Param_In const PathVertex& a,
			ls_Param_In const PathVertex& b);

		// ������������ľ���ƽ��
		static f32 distanceSquare(
			ls_Param_In const PathVertex& a,
			ls_Param_In const PathVertex& b);

		// ��������ĳ�� ��Դ����� �����ܶ�
		static f32 genLightPdf(
			ls_Param_In ls_Param_Out PathVertex& vertex,
			ls_Param_In const PathVertex& next);


		/*
			·������˳��:
			------------------->
			pre - > current -> next ( wo -> wi )
			  
		*/
		s32				next = -1;
		s32				pre = -1;

	private:
		PathVertexType		mVertexType;
		
	};

	class BiDirPath
	{
	public:
		BiDirPath(PathType type) :mPathType(type) {}

		~BiDirPath()
		{

		}
//		BiDirPath(const BiDirPath& var) = delete;
//		BiDirPath& operator=(const BiDirPath& var) = delete;

		auto getPathType() const { return mPathType; }
		auto size() const { return vertices.size(); }
		auto reserve(s32 cap) { return vertices.reserve(cap + 2); }


		void addVertex(const PathVertex& vertex)
		{	
			/*
				��������˳�������˳����ͬ�����������������෴

				pre ---wi----> cur  
			*/
			vertices.push_back(vertex);
			if (vertices.size() > 1)
			{
				PathVertex& pre = vertices[vertices.size() - 2];
				PathVertex& cur = vertices[vertices.size() - 1];

				pre.next = vertices.size() - 1;
				cur.pre = vertices.size() - 2;

//				cur.wo = normalize(Vec3(pre.position - cur.position));

				// �������ڶ���� ���������PDF
				pre.updatePdfReverse(*this);
				cur.updatePdfForward(*this);
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


		static BiDirPath createPathFromCamera(
			ls_Param_In SamplerPtr sampler,
			ls_Param_In ScenePtr scene,
			ls_Param_In const CameraSample& cameraSample,
			ls_Param_In const CameraPtr camera,
			ls_Param_In s32 maxDepth);

		static BiDirPath createPathFromLight(
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
			ls_Param_In ls_Param_Out BiDirPath* path);


		/*
			�ϲ����·���͹�Դ��·��
			��·���Ĵ������͸��ݲ���ָ�� ��Ĭ��Ϊ Radiance��
		*/
		static BiDirPath mergePath(
			const BiDirPath& cameraPath,
			const BiDirPath& lightPath,
			const s32 cameraPathSize,
			const s32 lightPathSize,
			PathType pathType = EPath_Camera);

		static BiDirPath mergePath(
			const BiDirPath& cameraPath,
			const LightSampleRecord& lightSampleRecord,
			f32 selectLightPdf,
			const s32 cameraPathSize,
			PathType pathType = EPath_Camera);


		// ���� �ϲ�·�� �е� p q ���� (p q ���ڲ�ͬ��������Ķ���)
		// Note: ֻ�������Ӷ���� pdf �������� throughput
		static void connectVertex(
			ls_Param_In ls_Param_Out BiDirPath& path,
			ls_Param_In s32 pIndex, s32 qIndex,
			ls_Param_In s32 prePIndex, s32 preQIndex);
		

	private:
		std::vector<PathVertex> vertices;

		PathType				mPathType;
	};
}