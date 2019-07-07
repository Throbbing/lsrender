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

bool ls::PathVertex::updatePdfForward(const Path& path)
{
	ls_AssertMsg(pre != -1, "PdfForward need valid Pre vertex! ");

	const auto& preVertex = path[pre];
	/*
		当前点为 环境光时 ，距离为无穷
		所以需要额外处理
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

bool ls::PathVertex::updatePdfReverse(const Path& path)
{
	ls_AssertMsg(next !=-1, "PdfReverse need valid Next vertex! ");
	const auto& nextVertex = path[next];
	/*
		当后续点是 环境光时，由于环境光在无穷远
		所以不能使用 基于面积 的概率密度
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
	ls_Param_Out LightSampleRecord * lsr)
{
	/*
		从光源创建Path Vertex，其传输类型一定为Importance
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
	vertex.wo = Vec3(0.f);//光源是Importance transport的端点，不会再反射
	vertex.pdfWi = lightSampleRecord.pdfDir;
	vertex.pdfWo = 0.f;
	vertex.pdfType = ScatteringFlag::EMeasure_SolidAngle;
	vertex.pdfForward = lightSampleRecord.pdfA; 
	vertex.pdfReverse = 0.f;
	vertex.throughput = 1.f;
	

	if (lsr)
		*lsr = lightSampleRecord;

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
		从相机创建Path Vertex，其传输类型一定为Radiance
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
	vertex.wo = Vec3(0.f);//相机是 Radiance Transport的端点，不会再反射 
	vertex.pdfWi = cameraSampleRecord.pdfD;
	vertex.pdfWo = 0.f;
	vertex.pdfType = ScatteringFlag::EMeasure_Area;
	vertex.pdfForward = cameraSampleRecord.pdfA;
	vertex.pdfReverse = 0.f;
	vertex.throughput = 1.f;

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
		Area Light 和 Env Light 相比，多了碰撞点和法线
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
	vertex.wo = Vec3(0.f);//光源是Importance transport的端点，不会再反射
	vertex.pdfWi = lightSampleRecord.pdfDir * lightSampleRecord.pdfA;
	vertex.pdfWo = 0.f;
	vertex.pdfType = ScatteringFlag::EMeasure_SolidAngle;
	vertex.pdfForward = 0.f;
	vertex.pdfReverse = 0.f;
	vertex.throughput = throughput;
	return vertex;
	
}

ls::PathVertex ls::PathVertex::createPathVertex(
	ls_Param_In TransportMode mode,
	ls_Param_In ScatteringFunctionPtr bsdf,
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
	vertex.scatter = bsdf;
	vertex.pdfType = ScatteringFlag::EMeasure_SolidAngle;
	vertex.pdfWi = surfaceSampleRecord.pdf;
	Frame localFrame(vertex.ns);
	auto wi = localFrame.toLocal(vertex.wi);
	auto wo = localFrame.toLocal(vertex.wo);
	vertex.pdfWo = bsdf->pdf(wo, wi);
	vertex.pdfForward = 0.f;
	vertex.pdfReverse = 0.f;
	vertex.throughput = throughput;

	return vertex;
}

ls::PathVertex ls::PathVertex::createPathVertex(
	ls_Param_In TransportMode mode, 
	ls_Param_In ScatteringFunctionPtr medium,
	ls_Param_In const MediumSampleRecord& msr,
	ls_Param_In const Spectrum& throughput)
{
	Unimplement;
	return PathVertex();
}



ls::Path ls::Path::createPathFromCamera(ls_Param_In SamplerPtr sampler,
	ls_Param_In ScenePtr scene,
	ls_Param_In const CameraSample & cameraSample,
	ls_Param_In const CameraPtr camera,
	ls_Param_In s32 maxDepth)
{
	Path path(EPath_Camera);
	Spectrum throughput(1.f);

	CameraSampleRecord cameraSampleRecord;
	auto cameraVertexStart = PathVertex::createPathVertex(
		TransportMode::ETransport_Radiance,
		sampler, cameraSample, camera, &cameraSampleRecord
	);

	/*
		在 Pinhole 中，该值为 1
		所以可以简化这个计算
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

ls::Path ls::Path::createPathFromLight(ls_Param_In SamplerPtr sampler,
	ls_Param_In ScenePtr scene,
	ls_Param_In s32 maxDepth)
{
	Path path(EPath_Light);
	Spectrum throughput(1.f);

	LightSampleRecord lightSampleRecord;
	auto selectedLightPdf = scene->sampleLight(sampler, &lightSampleRecord);
	auto light = lightSampleRecord.light;

	auto lightVertexStart = PathVertex::createPathVertex(
		TransportMode::ETransport_Importance,
		sampler, light, &lightSampleRecord);

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

void ls::Path::randomWalk(ls_Param_In ScenePtr scene,
	ls_Param_In SamplerPtr sampler, 
	ls_Param_In const DifferentialRay& ray,
	ls_Param_In Spectrum throughput, 
	ls_Param_In TransportMode transMode, 
	ls_Param_In s32 maxDepth, 
	ls_Param_In ls_Param_Out Path * path)
{
	s32 depth = 1;
	DifferentialRay castRay(ray);
	IntersectionRecord itsRec;
	while (depth < maxDepth)
	{
		if (throughput.maxComponent() < lsMath::Epsilon)
			return;

		if (!scene->intersect(castRay, &itsRec))
		{
			//判断场景是否含有 Env Light
			
			if (scene->envrionmentLight())
			{
				//生成 Env Light Vertex
				
				//空 IntersectionRecord 为了参数传递
				IntersectionRecord emptyIts;

				auto vertex = PathVertex::createPathVertex(
					transMode,
					scene->envrionmentLight(),
					castRay,
					emptyIts,
					throughput);

				path->addVertex(vertex);
				return;
			}
		}//endif !scene->intersect(castRay, &its)

		//碰撞点为 面积光源
		if (itsRec.areaLight)
		{
			//生成 面积光源 顶点
			auto vertex = PathVertex::createPathVertex(
				transMode,
				itsRec.areaLight,
				castRay,
				itsRec,
				throughput);

			path->addVertex(vertex);

			// 面积光源 顶点 必定为路径的端点
			// 因为在 lsrender 中，光源不会反射光线
			return;

		}// endif itsRec.areaLight


		/*
			Surface or Medium

			暂时不处理 Medium
		*/
		if (!itsRec.material)
			return;

		auto bsdf = itsRec.material->getSurfaceScattering();
		if (!bsdf)
			return;

		//采样该碰撞表面的BSDF
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

		//生成新的顶点
		auto newVertex = PathVertex::createPathVertex(transMode,
			bsdf, surfaceSampleRecord, throughput);
		path->addVertex(newVertex);


		//更新 throughput
		throughput *= bsdfVal *
			std::fabs(dot(surfaceSampleRecord.normal, surfaceSampleRecord.wi))
			/ bsdfPdfW;

		//矫正由于着色法线导致的 BSDF 非对称
		throughput *= RenderLib::correctShadingNormal(
			surfaceSampleRecord.wi,
			surfaceSampleRecord.wo,
			itsRec.ng,
			itsRec.ns,
			transMode);

		//更新 castRay
		castRay = DifferentialRay(itsRec.position,
			surfaceSampleRecord.wi, depth + 1);

		//更新深度
		depth++;

	}// while (depth < maxDepth)
}


bool ls::PathVertex::connectable(
	ls_Param_In const PathVertex& a,
	ls_Param_In const PathVertex& b)
{
	//
	auto aType = a.getVertexType();
	auto bType = b.getVertexType();

	/*
		必须都为 Surface Vertex
		暂时不考虑 Medium
	*/
	if (aType != PathVertexType::EPathVertex_Surface ||
		bType != PathVertexType::EPathVertex_Surface)
	{
		return false;
	}

	/*
		连接 S 顶点的路径贡献值为 0
		所以 S 顶点不可连
	*/
	if ((a.pathVertexRecord.surfaceSampleRecord.scatterFlag &
		EScattering_S) ||
		(b.pathVertexRecord.surfaceSampleRecord.scatterFlag &
		EScattering_S) )
	{
		return false;
	}
	return true;
}