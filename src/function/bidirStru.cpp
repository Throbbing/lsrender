#include<config/common.h>
#include<camera/camera.h>
#include<camera/pinhole.h>
#include<function/bidirStru.h>
#include<light/light.h>
#include<material/material.h>
#include<scene/scene.h>

f32 ls::PathVertex::getImportancePdf() const
{
	if (transmode == TransportMode::ETransport_Importance)
	{
		return pdfForward;
	}
	else
	{
		return pdfReverse;
	}
}



f32 ls::PathVertex::getRadiancePdf() const
{
	if (transmode == TransportMode::ETransport_Radiance)
	{
		return pdfForward;
	}
	else
	{
		return pdfReverse;
	}
}

bool ls::PathVertex::updatePdfForward(const BiDirPath& path)
{
	ls_AssertMsg(pre != -1, "PdfForward need valid Pre vertex! ");

	const auto& preVertex = path[pre];
	/*
		��ǰ��Ϊ ������ʱ ������Ϊ����
		������Ҫ���⴦��
	*/
	if (getVertexType() == PathVertexType::EPathVertex_Light &&
		pathVertexRecord.lightSampleRecord.light->getLightType() ==
		LightType::ELight_Environment)
	{
		pdfForward = preVertex.pdfWi;
		return true;
	}


	auto dir = Vec3(preVertex.position - position);
	auto dist = dir.length();
	if (dist == 0.f) return false;
	dir /= dist;

	pdfForward = RenderLib::pdfW2A(preVertex.pdfWi, dist, std::fabs(dot(ns, dir)));
	return true;
}

bool ls::PathVertex::updatePdfReverse(const BiDirPath& path)
{
	ls_AssertMsg(next !=-1, "PdfReverse need valid Next vertex! ");
	const auto& nextVertex = path[next];
	/*
		���������� ������ʱ�����ڻ�����������Զ
		���Բ���ʹ�� ������� �ĸ����ܶ�
	*/
	
	if (nextVertex.getVertexType() == PathVertexType::EPathVertex_Light &&
		nextVertex.pathVertexRecord.lightSampleRecord.light->getLightType()==
		LightType::ELight_Environment)
	{
		pdfReverse = nextVertex.pdfWo;
		return true;
	}

	auto dir = Vec3(nextVertex.position - position);
	auto dist = dir.length();
	if (dist == 0.f) return false;
	dir /= dist;

	pdfReverse = RenderLib::pdfW2A(nextVertex.pdfWo, dist, std::fabs(dot(ns, dir)));
	return true;
}

bool ls::PathVertex::isConnectable() const
{
	// ֻ�� �� delta ����ſ�������
	if (mVertexType != EPathVertex_Surface)
		return false;

	auto bsdf = material->getSurfaceScattering();

	if (bsdf->isDelta())
		return false;

	return true;
	
}
void ls::PathVertex::getDirection(ls_Param_In TransportMode mode, ls_Param_Out Vec3 * wi, ls_Param_Out Vec3 * wo)
{
	if (mode == transmode)
	{
		*wi = this->wi;
		*wo = this->wo;
	}
	else
	{
		*wi = this->wo;
		*wo = this->wi;
	}
}

ls::PathVertex ls::PathVertex::createPathVertex(
	ls_Param_In TransportMode mode,
	ls_Param_In SamplerPtr sampler,
	ls_Param_In LightPtr light, 
	ls_Param_In f32 selectLightPdf,
	ls_Param_Out LightSampleRecord * lsr)
{
	/*
		�ӹ�Դ���� BiDirPath Vertex���䴫������һ��ΪImportance
	*/
	ls_AssertMsg(mode == TransportMode::ETransport_Importance, "The type of PathVertex created from Light should be Importance");

	LightSampleRecord lightSampleRecord;

	light->sample(sampler, &lightSampleRecord);

	PathVertex vertex;
	
	
	vertex.transmode = mode;
	vertex.mVertexType = EPathVertex_Light;
	vertex.pathVertexRecord.lightSampleRecord = lightSampleRecord;
	vertex.position = lightSampleRecord.samplePosition;
	vertex.ns = lightSampleRecord.n;
	vertex.wi = lightSampleRecord.sampleDirection;
	vertex.wo = Vec3(0.f);//��Դ��Importance transport�Ķ˵㣬�����ٷ���
	vertex.pdfWi = lightSampleRecord.pdfDir;
	vertex.pdfWo = 0.f;
	vertex.pdfType = ScatteringFlag::EMeasure_SolidAngle;
	vertex.pdfForward = lightSampleRecord.pdfPos * selectLightPdf; 
	vertex.pdfReverse = 0.f;
	vertex.throughput = 1.f;

	vertex.material = light->getDummyMaterial();
	

	if (lsr)
		*lsr = lightSampleRecord;

	return vertex;
}

ls::PathVertex ls::PathVertex::createPathVertex(
	ls_Param_In TransportMode mode,
	ls_Param_In f32 selectLightPdf,
	ls_Param_In const Point3& refP,
	ls_Param_In const LightSampleRecord& lsr)
{
	ls_AssertMsg(mode == TransportMode::ETransport_Importance, "The type of PathVertex created from Light should be Importance");
	
	PathVertex vertex;
	vertex.transmode = mode;
	vertex.mVertexType = EPathVertex_Light;
	vertex.pathVertexRecord.lightSampleRecord = lsr;
	vertex.position = lsr.samplePosition;
	vertex.ns = lsr.n;
	vertex.wi = lsr.sampleDirection;
	vertex.wo = Vec3(0.f);//��Դ��Importance transport�Ķ˵㣬�����ٷ���
	vertex.pdfWi = lsr.pdfW;
	vertex.pdfWo = 0.f;
	vertex.pdfType = ScatteringFlag::EMeasure_SolidAngle;

	Ray toLightRay = Ray(refP, normalize(Vec3(lsr.samplePosition - refP)));
	IntersectionRecord its;
	its.position = lsr.samplePosition;
	its.ns = lsr.n;
	its.ng = lsr.n;

	LightSampleRecord copyLSR = lsr;
	lsr.light->pdf(&copyLSR);
	vertex.pdfForward = copyLSR.pdfPos * selectLightPdf;
	vertex.pdfReverse = 0.f;
	vertex.throughput = 1.f;

	vertex.material = lsr.light->getDummyMaterial();

	return vertex;
}

ls::PathVertex ls::PathVertex::createPathVertex(
	ls_Param_In TransportMode mode,
	ls_Param_In SamplerPtr sampler, 
	ls_Param_In const CameraSample & cameraSample, 
	ls_Param_In CameraPtr camera, 
	ls_Param_Out CameraSampleRecord * csr)
{
	/*
		���������BiDirPath Vertex���䴫������һ��ΪRadiance
	*/
	ls_AssertMsg(mode == TransportMode::ETransport_Radiance, "The type of PathVertex created from Camera should be Radiance");
	CameraSampleRecord cameraSampleRecord;

	camera->sample(sampler, cameraSample,&cameraSampleRecord);
	
	PathVertex vertex;
	vertex.transmode = mode;
	vertex.mVertexType = EPathVertex_Camera;
	vertex.pathVertexRecord.cameraSampleRecord = cameraSampleRecord;
	vertex.position = cameraSampleRecord.samplePosition;
	vertex.ns = cameraSampleRecord.n;
	vertex.wi = cameraSampleRecord.sampleDirection;
	vertex.wo = Vec3(0.f);//����� Radiance Transport�Ķ˵㣬�����ٷ��� 
	vertex.pdfWi = cameraSampleRecord.pdfD;
	vertex.pdfWo = 0.f;
	vertex.pdfType = ScatteringFlag::EMeasure_Area;
	vertex.pdfForward = cameraSampleRecord.pdfA;
	vertex.pdfReverse = 0.f;
	vertex.throughput = 1.f;

	vertex.material = camera->getDummyMaterial();

	if (csr)
		*csr = cameraSampleRecord;

	

	return vertex;
}

ls::PathVertex ls::PathVertex::createPathVertex(
	ls_Param_In TransportMode mode,
	ls_Param_In LightPtr light, 
	ls_Param_In const DifferentialRay& ray,
	ls_Param_In const IntersectionRecord & itsRecord, 
	ls_Param_In const Spectrum & throughput)
{

	LightSampleRecord lightSampleRecord;
	lightSampleRecord.light = light;
	lightSampleRecord.samplePosition = Point3(0.f);
	lightSampleRecord.n = Normal(0.f);

	/*
		Area Light �� Env Light ��ȣ�������ײ��ͷ���
	*/
	if (light->getLightType() == LightType::ELight_Area)
	{
		lightSampleRecord.samplePosition = itsRecord.position;
		lightSampleRecord.n = itsRecord.ns;
	}
	lightSampleRecord.sampleDirection = -ray.dir;
	lightSampleRecord.le = light->sample(ray,itsRecord);
	lightSampleRecord.pdfPos = 1.f;
	lightSampleRecord.pdfDir = light->pdf(ray, itsRecord);



	PathVertex vertex;
	vertex.transmode = mode;
	vertex.mVertexType = EPathVertex_Light;
	vertex.pathVertexRecord.lightSampleRecord = lightSampleRecord;
	vertex.position = lightSampleRecord.samplePosition;
	vertex.ns = lightSampleRecord.n;
	vertex.wi = lightSampleRecord.sampleDirection;
	vertex.wo = Vec3(0.f);//��Դ��Importance transport�Ķ˵㣬�����ٷ���
	vertex.pdfWi = 0.f;
	vertex.pdfWo = 0.f;
	vertex.pdfType = ScatteringFlag::EMeasure_SolidAngle;
	vertex.pdfForward = 0.f;
	vertex.pdfReverse = 0.f;
	vertex.throughput = throughput;

	vertex.material = light->getDummyMaterial();
	return vertex;
	
}

ls::PathVertex ls::PathVertex::createPathVertex(
	ls_Param_In TransportMode mode,
	ls_Param_In const IntersectionRecord& its,
	ls_Param_In const SurfaceSampleRecord& surfaceSampleRecord,
	ls_Param_In const Spectrum& throughput)
{
	PathVertex vertex;

	vertex.transmode = mode;
	vertex.mVertexType = EPathVertex_Surface;
	vertex.pathVertexRecord.surfaceSampleRecord = surfaceSampleRecord;
	vertex.position = surfaceSampleRecord.position;
	vertex.ns = surfaceSampleRecord.normal;
	vertex.wi = surfaceSampleRecord.wi;
	vertex.wo = surfaceSampleRecord.wo;
	vertex.material = its.material;
	vertex.pdfType = ScatteringFlag::EMeasure_SolidAngle;
	vertex.pdfWi = surfaceSampleRecord.pdf;

	Frame localFrame(vertex.ns);
	auto wi = localFrame.toLocal(vertex.wi);
	auto wo = localFrame.toLocal(vertex.wo);
	
	vertex.pdfWo = its.material->getSurfaceScattering()->pdf(wo, wi);
	vertex.pdfForward = 0.f;
	vertex.pdfReverse = 0.f;
	vertex.throughput = throughput;

	// ������ײ��
	vertex.its = its;

	return vertex;
}

ls::PathVertex ls::PathVertex::createPathVertex(
	ls_Param_In TransportMode mode, 
	ls_Param_In MaterialPtr medium,
	ls_Param_In const MediumSampleRecord& msr,
	ls_Param_In const Spectrum& throughput)
{
	Unimplement;
	return PathVertex();
}



ls::BiDirPath ls::BiDirPath::createPathFromCamera(ls_Param_In SamplerPtr sampler,
	ls_Param_In ScenePtr scene,
	ls_Param_In const CameraSample & cameraSample,
	ls_Param_In const CameraPtr camera,
	ls_Param_In s32 maxDepth)
{
	BiDirPath path(EPath_Camera);
	Spectrum throughput(1.f);

	CameraSampleRecord cameraSampleRecord;
	auto cameraVertexStart = PathVertex::createPathVertex(
		TransportMode::ETransport_Radiance,
		sampler, cameraSample, camera, &cameraSampleRecord
	);

	/*
		�� Pinhole �У���ֵΪ 1
		���Կ��Լ��������
	*/
	
	if (typeid(*camera) != typeid(Pinhole))
	{
		throughput *= cameraSampleRecord.we *
			std::fabs(dot(camera->look(), cameraSampleRecord.sampleDirection))
			/ (cameraSampleRecord.pdfD * cameraSampleRecord.pdfA);
	}
	else
	{
		throughput *= 1.f;
	}
	path.addVertex(cameraVertexStart);
	DifferentialRay ray(Ray(cameraSampleRecord.samplePosition,
		cameraSampleRecord.sampleDirection));

	randomWalk(scene,
		sampler,
		ray,
		throughput,
		TransportMode::ETransport_Radiance,
		maxDepth,
		&path);

	return path;
	
}

ls::BiDirPath ls::BiDirPath::createPathFromLight(ls_Param_In SamplerPtr sampler,
	ls_Param_In ScenePtr scene,
	ls_Param_In s32 maxDepth)
{
	BiDirPath path(EPath_Light);
	Spectrum throughput(1.f);

	LightSampleRecord lightSampleRecord;
	auto selectedLightPdf = scene->sampleLight(sampler, &lightSampleRecord);
	auto light = lightSampleRecord.light;

	auto lightVertexStart = PathVertex::createPathVertex(
		TransportMode::ETransport_Importance,
		sampler, light,selectedLightPdf, &lightSampleRecord);

	throughput *= lightSampleRecord.le /
		(lightSampleRecord.pdfPos * lightSampleRecord.pdfDir * selectedLightPdf);

	path.addVertex(lightVertexStart);
	DifferentialRay ray(Ray(lightSampleRecord.samplePosition,
		lightSampleRecord.sampleDirection));

	randomWalk(scene,
		sampler,
		ray,
		throughput,
		TransportMode::ETransport_Importance,
		maxDepth,
		&path);

	return path;
	
}

void ls::BiDirPath::randomWalk(ls_Param_In ScenePtr scene,
	ls_Param_In SamplerPtr sampler, 
	ls_Param_In const DifferentialRay& ray,
	ls_Param_In Spectrum throughput, 
	ls_Param_In TransportMode transMode, 
	ls_Param_In s32 maxDepth, 
	ls_Param_In ls_Param_Out BiDirPath * path)
{
	s32 depth = 1;
	DifferentialRay castRay(ray);
	IntersectionRecord itsRec;
	while (depth < maxDepth)
	{
		if (throughput.maxComponent() < lsMath::Epsilon)
			return;

		clearMemory(&itsRec, sizeof(itsRec));
		if (!scene->intersect(castRay, &itsRec))
		{
			//�жϳ����Ƿ��� Env Light
			
			if (scene->envrionmentLight())
			{
				//���� Env Light Vertex
				
				//�� IntersectionRecord Ϊ�˲�������
				IntersectionRecord emptyIts;

				auto vertex = PathVertex::createPathVertex(
					transMode,
					scene->envrionmentLight(),
					castRay,
					emptyIts,
					throughput);

				path->addVertex(vertex);

			}
			return;
		}//endif !scene->intersect(castRay, &its)

		//��ײ��Ϊ �����Դ
		if (itsRec.areaLight)
		{
			//���� �����Դ ����
			auto vertex = PathVertex::createPathVertex(
				transMode,
				itsRec.areaLight,
				castRay,
				itsRec,
				throughput);

			path->addVertex(vertex);

			// �����Դ ���� �ض�Ϊ·���Ķ˵�
			// ��Ϊ�� lsrender �У���Դ���ᷴ�����
			return;

		}// endif itsRec.areaLight


		/*
			Surface or Medium

			��ʱ������ Medium
		*/
		if (!itsRec.material)
			return;

		auto bsdf = itsRec.material->getSurfaceScattering();
		if (!bsdf)
			return;

		//��������ײ�����BSDF
		SurfaceSampleRecord surfaceSampleRecord;
		clearMemory(&surfaceSampleRecord, sizeof(surfaceSampleRecord));
		RenderLib::fillScatteringRecordForBSDFSample(
			itsRec.position,
			itsRec.ns,
			-castRay.dir,
			transMode,
			&surfaceSampleRecord);

		RenderLib::sampleSurfaceBSDF(sampler, bsdf, &surfaceSampleRecord);

		auto bsdfPdfW = surfaceSampleRecord.pdf;
		auto bsdfVal = surfaceSampleRecord.sampledValue *
			itsRec.material->scatteringFactor(surfaceSampleRecord, itsRec);


		if (bsdfVal.isBlack())
			return;

		//�����µĶ���
		auto newVertex = PathVertex::createPathVertex(transMode,
			itsRec, surfaceSampleRecord, throughput);


		path->addVertex(newVertex);


		//���� throughput
		throughput *= bsdfVal *
			std::fabs(dot(surfaceSampleRecord.normal, surfaceSampleRecord.wi))
			/ bsdfPdfW;

		//����������ɫ���ߵ��µ� BSDF �ǶԳ�
		throughput *= RenderLib::correctShadingNormal(
			surfaceSampleRecord.wi,
			surfaceSampleRecord.wo,
			itsRec.ng,
			itsRec.ns,
			transMode);

		//���� castRay
		castRay = DifferentialRay(itsRec.position,
			surfaceSampleRecord.wi, depth + 1);

		//�������
		depth++;

	}// while (depth < maxDepth)
}



ls::BiDirPath ls::BiDirPath::mergePath(const BiDirPath & cameraPath,
	const BiDirPath & lightPath, 
	const s32 cameraPathSize, 
	const s32 lightPathSize, 
	PathType pathType)
{
#if 1
	BiDirPath mergedPath(pathType);

	ls_Assert(cameraPath.size() >= cameraPathSize);
	ls_Assert(lightPath.size() >= lightPathSize);
	ls_Assert(cameraPathSize > 0);
	// �ϲ�·��
	if (pathType = EPath_Camera)
	{
		for (s32 i = 0; i < cameraPathSize; ++i)
		{
			mergedPath.vertices.push_back(cameraPath[i]);
		}
		for (s32 i = lightPathSize - 1; i >= 0; --i)
		{
			mergedPath.vertices.push_back(lightPath[i]);
		}
		
		if (lightPathSize > 0)
		{
		
			BiDirPath::connectVertex(mergedPath,
				cameraPathSize - 1, cameraPathSize,
				cameraPathSize - 2 >= 0 ? cameraPathSize - 2 : -1,
				cameraPathSize + 1 < mergedPath.size() ? cameraPathSize + 1 : -1);
		}
		else if(mergedPath.size() >= 2)
		{
			auto& tVertex = mergedPath[mergedPath.size() - 1];
			auto& preTVertex = mergedPath[mergedPath.size() - 2];


			tVertex.pdfReverse = PathVertex::genLightPdf(tVertex, preTVertex);

			auto dir = Vec3(tVertex.position - preTVertex.position);
			auto dist = dir.length();
			ls_Assert(dist != 0.f);
			dir /= dist;
			preTVertex.pdfReverse = RenderLib::pdfW2A(tVertex.pathVertexRecord.lightSampleRecord.pdfDir,
				dist,
				std::fabs(dot(preTVertex.ns, dir)));
		}
		
	}
	else
	{
		for (s32 i = 0; i < lightPathSize; ++i)
		{
			mergedPath.vertices.push_back(lightPath[i]);
		}
		for (s32 i = cameraPathSize - 1; i >= 0; --i)
		{
			mergedPath.vertices.push_back(cameraPath[i]);
		}
		
		if (lightPathSize > 0)
		{
			BiDirPath::connectVertex(mergedPath,
				lightPathSize - 1, lightPathSize,
				lightPathSize - 2 >= 0 ? lightPathSize - 2 : -1,
				lightPathSize + 1 < mergedPath.size() ? lightPathSize + 1 : -1);
		}
		else if (mergedPath.size() >= 2)
		{
			auto& tVertex = mergedPath[0];
			auto& preTVertex = mergedPath[1];

			
			tVertex.pdfReverse = PathVertex::genLightPdf(tVertex, preTVertex);
			
			auto dir = Vec3(tVertex.position - preTVertex.position);
			auto dist = dir.length();
			ls_Assert(dist != 0.f);
			dir /= dist;
			preTVertex.pdfReverse = RenderLib::pdfW2A(tVertex.pathVertexRecord.lightSampleRecord.pdfDir,
				dist,
				std::fabs(dot(preTVertex.ns, dir)));
		}
	}
	return mergedPath;
#endif
}

ls::BiDirPath ls::BiDirPath::mergePath(const BiDirPath & cameraPath, 
	const LightSampleRecord & lightSampleRecord, 
	f32 selectLightPdf,
	const s32 cameraPathSize, PathType pathType)
{


	ls_Assert(cameraPath[cameraPathSize - 1].getVertexType() == EPathVertex_Surface);
	auto lightVertex = PathVertex::createPathVertex(
		ETransport_Importance,
		selectLightPdf,
		cameraPath[cameraPathSize - 1].position,
		lightSampleRecord);

	BiDirPath mergedPath(pathType);

	if (pathType = EPath_Camera)
	{
		for (s32 i = 0; i < cameraPathSize; ++i)
		{
			mergedPath.vertices.push_back(cameraPath[i]);
		}
		mergedPath.vertices.push_back(lightVertex);
		
		BiDirPath::connectVertex(mergedPath,
			cameraPathSize - 1, cameraPathSize,
			cameraPathSize - 2 >= 0 ? cameraPathSize - 2 : -1,
			cameraPathSize + 1 < mergedPath.size() ? cameraPathSize + 1 : -1);
	}
	else
	{
		mergedPath.vertices.push_back(lightVertex);
		for (s32 i = cameraPathSize - 1; i >= 0; --i)
		{
			mergedPath.vertices.push_back(cameraPath[i]);
		}

		BiDirPath::connectVertex(mergedPath,
			0, 1,
			-1,
			2 < mergedPath.size() ? 2 : -1);

	}

	return mergedPath;
}


bool ls::PathVertex::connectable(
	ls_Param_In const PathVertex& a,
	ls_Param_In const PathVertex& b)
{
	//
	auto aType = a.getVertexType();
	auto bType = b.getVertexType();

	s32 surfaceCount = 0;
	if (aType == EPathVertex_Surface)
		++surfaceCount;
	if (bType == EPathVertex_Surface)
		++surfaceCount;

	// �������㶼Ϊ ������ײ��
	if (surfaceCount == 2)
	{
		// ���� S �����򲻿�������
		return !(a.pathVertexRecord.surfaceSampleRecord.scatterFlag &
			EScattering_S) ||
			(b.pathVertexRecord.surfaceSampleRecord.scatterFlag &
				EScattering_S);
	}
	else if (surfaceCount == 1)
	{
		
		const PathVertex* surfaceVertex;
		const PathVertex* noSurfaceVertex;
		if (aType == EPathVertex_Surface)
		{
			surfaceVertex = &a;
			noSurfaceVertex = &b;
		}
		else
		{
			surfaceVertex = &b;
			noSurfaceVertex = &a;
		}

		// �жϷǱ��涥�������
		if (noSurfaceVertex->getVertexType() == EPathVertex_Light)
		{
			// ��Դ���� �ж��Ƿ��� delta ��Դ
			// delta ��Դ������
			return !noSurfaceVertex->pathVertexRecord.lightSampleRecord.light->isDelta();
		}
		
		return true;
	}

	// Note: ��ʱ��֧�� ����
	return false;
}

f32 ls::PathVertex::distance(ls_Param_In const PathVertex & a, ls_Param_In const PathVertex & b)
{
	return Vec3(b.position - a.position).length();
}

f32 ls::PathVertex::distanceSquare(ls_Param_In const PathVertex & a, ls_Param_In const PathVertex & b)
{
	return Vec3(b.position - a.position).length2();
}

f32 ls::PathVertex::genLightPdf(ls_Param_In PathVertex & vertex, ls_Param_In const PathVertex & next)
{
	ls_Assert(vertex.getVertexType() == EPathVertex_Light);

	auto light = vertex.pathVertexRecord.lightSampleRecord.light;
	ls_Assert(light);

	f32 pdf = 0.f;
	if (light->getLightType() == ELight_Environment)
	{
		// ��������ʱ��֧��
		ls_AssertMsg(false, "Environment Light has not supported yet");
	}
	else
	{
		light->pdf(&vertex.pathVertexRecord.lightSampleRecord);
		pdf = vertex.pathVertexRecord.lightSampleRecord.pdfPos;
	}

	f32 selectLightPdf = lsRender::scene->pdfSampleLight(light);

	return pdf * selectLightPdf;


}

void ls::BiDirPath::connectVertex(
	ls_Param_In ls_Param_Out BiDirPath& path,
	ls_Param_In s32 pIndex, s32 qIndex,
	ls_Param_In s32 prePIndex, s32 preQIndex)
{
	auto& pVertex = path[pIndex];
	auto& qVertex = path[qIndex];

	auto pBSDF = pVertex.material->getSurfaceScattering();
	auto qBSDF = qVertex.material->getSurfaceScattering();

	// ���� p q �������µ� wi
	pVertex.wi = Vec3(qVertex.position - pVertex.position);
	qVertex.wi = Vec3(pVertex.position - qVertex.position); // -pVertex.wi
	f32 pqDist = pVertex.wi.length();
	pVertex.wi /= pqDist;
	qVertex.wi /= pqDist;

	// ���� p ����� pdfWi pdfWo
	Frame pLocalFrame = Frame(pVertex.ns);
	auto plWi = pLocalFrame.toLocal(pVertex.wi);
	auto plWo = pLocalFrame.toLocal(pVertex.wo);
	pVertex.pdfWi = pBSDF->pdf(plWi, plWo);
	pVertex.pdfWo = pBSDF->pdf(plWo, plWi);
	
	// ���� p ��ǰ�ö���� pdfReverse
	if (prePIndex != -1)
	{
		auto& prePVertex = path[prePIndex];
		auto dir = Vec3(pVertex.position - prePVertex.position);
		auto dist = dir.length();
		ls_Assert(dist != f32(0));
		dir /= dist;

		prePVertex.pdfReverse = RenderLib::pdfW2A(pVertex.pdfWo,
			dist,
			std::fabs(dot(prePVertex.ns, dir)));
	}

	// ���� q ����� pdfWi pdfWo
	Frame qLocalFrame = Frame(qVertex.ns);
	auto qlWi = qLocalFrame.toLocal(qVertex.wi);
	auto qlWo = qLocalFrame.toLocal(qVertex.wo);
	qVertex.pdfWi = qBSDF->pdf(qlWi, qlWo);
	qVertex.pdfWo = qBSDF->pdf(qlWo, qlWi);

	// ���� q ��ǰ�ö���� pdfReverse
	if (preQIndex != -1)
	{
		auto& preQVertex = path[preQIndex];
		auto dir = Vec3(qVertex.position - preQVertex.position);
		auto dist = dir.length();
		ls_Assert(dist != f32(0));
		dir /= dist;

		preQVertex.pdfReverse = RenderLib::pdfW2A(qVertex.pdfWo,
			dist,
			std::fabs(dot(preQVertex.ns, dir)));
	}

	// ���� p q ����� pdfReverse 
	pVertex.pdfReverse = RenderLib::pdfW2A(qVertex.pdfWi,
		pqDist,
		std::fabs(dot(pVertex.ns, pVertex.wi)));
	qVertex.pdfReverse = RenderLib::pdfW2A(pVertex.pdfWi,
		pqDist,
		std::fabs(dot(qVertex.ns, qVertex.wi)));
}
