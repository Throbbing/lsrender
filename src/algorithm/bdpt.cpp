#include<config/lsPtr.h>
#include<algorithm/bdpt.h>
#include<record/record.h>
#include<function/stru.h>
#include<function/bidirStru.h>
#include<scene/scene.h>
#include<light/light.h>
#include<material/material.h>
#include<resource/xmlHelper.h>
#include<resource/resourceManager.h>
#include<sampler/sampler.h>
#include<spectrum/spectrum.h>
#include<scatter/scatter.h>


void ls::BDPT::render(ScenePtr scene, SceneRenderBlock * renderBlock, SamplerPtr sampler, CameraPtr camera, RNG & rng) const
{
}

ls::RenderAlgorithmPtr ls::BDPT::copy() const
{
	return RenderAlgorithmPtr();
}

ls::Spectrum ls::BDPT::Li(ls_Param_In const DifferentialRay ray, 
	ls_Param_In s32 depth, 
	ls_Param_In CameraSampleRecord * cameraSampleRec, ls_Param_In Scene * scene, ls_Param_In Sampler * sampler, ls_Param_In RNG & rng, ls_Param_In MemoryAllocater * arena) const
{
	return ls::Spectrum();
}

std::string ls::BDPT::strOut() const
{
	return std::string();
}

ls::Spectrum ls::BDPT::connectPath(ls_Param_In ScenePtr scene,
	ls_Param_In SamplerPtr sampler,
	ls_Param_In const Path & lightPath, 
	ls_Param_In const Path & cameraPath, 
	ls_Param_In s32 s,
	ls_Param_In s32 t,
	ls_Param_In ls_Param_Out CameraSample * cameraSample)
{
	if (t == 0)
		return Spectrum(0.f);

	Spectrum L(0.f);
	/*
		case 1: s = 0 ,ֱ�ӹ���
	*/
	if (s == 0)
	{
		//ֱ�ӹ��գ�ֱ�Ӽ��� ��Դ����� Le
		auto& v = cameraPath[t - 1];
		if (v.getVertexType() != PathVertexType::EPathVertex_Light)
			return 0.f;

		L = v.throughput * v.pathVertexRecord.lightSampleRecord.le;
	}
	/*
		case 2: s = 1,��Ҫ�ж� Surface ����� ��Դ �����Ƿ��������
	*/
	else if (s == 1)
	{

	}
	/*
		case 3: t = 1  ֻ��һ��������㣬������Ҫ���¼��� ���� λ��
	*/
	else if (t == 1)
	{

	}
	/*
		case 4: ��������� BDPT General ���ӷ�ʽ
	*/
	else
	{
		auto& cameraVertex = cameraPath[t - 1];
		auto& lightVertex = lightPath[s - 1];

		if (!PathVertex::connectable(cameraVertex, lightVertex))
			return Spectrum(0.f);

		L = cameraVertex.throughput * cameraVertex.
	}
}

ls::BDPT::BDPT(ParamSet & paramSet)
{
}
