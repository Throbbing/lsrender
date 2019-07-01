#include<camera/camera.h>
#include<light/light.h>
#include<function/bidirStru.h>

f32 ls::PathVertex::getImportancePdf()
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

f32 ls::PathVertex::getRadiancePdf()
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
	vertex.wi = -lightSampleRecord.sampleDirection;
	vertex.wo = Vec3(0);//光源是Importance transport的端点，不会再反射
	vertex.pdfType = ScatteringFlag::EMeasure_SolidAngle;
	vertex.pdfForward = 1.f;//  无 wo 方向
	vertex.pdfReverse = lightSampleRecord.pdfDir * lightSampleRecord.pdfA;
	

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
	vertex.wi = -cameraSampleRecord.sampleDirection;
	vertex.wo = Vec3(0.f);//相机是 Radiance Transport的端点，不会再反射 
	vertex.pdfType = ScatteringFlag::EMeasure_Area;
	vertex.pdfForward = 1.f; // 无 wo 方向
	vertex.pdfReverse = cameraSampleRecord.pdfD * cameraSampleRecord.pdfA;

	if (csr)
		*csr = cameraSampleRecord;

	return vertex;
}

ls::PathVertex ls::PathVertex::createPathVertex(
	ls_Param_In TransportMode mode,
	ls_Param_In ScatteringFunctionPtr bsdf,
	ls_Param_In SurfaceSampleRecord surfaceSampleRecord)
{
	PathVertex vertex;

	vertex.transmode = mode;
	vertex.mVertexType = EPathVertex_Surface;
	vertex.pathVertexRecord.surfaceSampleRecord = surfaceSampleRecord;
	vertex.position = surfaceSampleRecord.position;
	vertex.ns = surfaceSampleRecord.normal;
	vertex.wi = surfaceSampleRecord.wi;
	vertex.wo = surfaceSampleRecord.wo;
	vertex.pdfType = ScatteringFlag::EMeasure_SolidAngle;
	if (mode == TransportMode::ETransport_Radiance)
	{
		vertex.pdfForward = surfaceSampleRecord.pdfRadiance;
		Frame localFrame(vertex.ns);
		auto wi = localFrame.toLocal(vertex.wi);
		auto wo = localFrame.toLocal(vertex.wo);
		vertex.pdfReverse = bsdf->pdf(wo, wi);
	}
	else
	{
		vertex.pdfForward = surfaceSampleRecord.pdfImportance;
		Frame localFrame(vertex.ns);
		auto wi = localFrame.toLocal(vertex.wi);
		auto wo = localFrame.toLocal(vertex.wo);
		vertex.pdfReverse = bsdf->pdf(wo, wi);
	}
	return vertex;
}

ls::PathVertex ls::PathVertex::createPathVertex(
	ls_Param_In TransportMode mode, 
	ls_Param_In ScatteringFunctionPtr medium,
	ls_Param_In MediumSampleRecord msr)
{
	Unimplement;
	return PathVertex();
}

ls::Path ls::Path::createPathFromCamera(ls_Param_In SamplerPtr sampler, ls_Param_In const CameraSample & cameraSample, ls_Param_In const CameraPtr camera)
{
	Path path;
}

ls::Path ls::Path::createPathFromLight(ls_Param_In SamplerPtr sampler, ls_Param_In ScenePtr scene)
{
	return Path();
}
