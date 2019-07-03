#pragma once

#include<config/declaration.h>
#include<algorithm/renderalgorithm.h>
#include<spectrum/spectrum.h>
#include<function/func.h>
namespace ls
{
	class BDPT :public RenderAlgorithm
	{
		friend ResourceManager;
	public:
		BDPT(s32 maxDepth,
			const std::string& id = "PathTracer") :RenderAlgorithm(id), mPathMaxDepth(maxDepth) {}
		virtual ~BDPT() {}

		virtual void render(ScenePtr scene,
			SceneRenderBlock* renderBlock,
			SamplerPtr sampler,
			CameraPtr camera, RNG& rng) const override;

		virtual RenderAlgorithmPtr copy() const override;
		virtual ls::Spectrum Li(ls_Param_In const DifferentialRay ray,
			ls_Param_In s32	depth,
			ls_Param_In CameraSampleRecord* cameraSampleRec,
			ls_Param_In Scene* scene,
			ls_Param_In Sampler* sampler,
			ls_Param_In RNG& rng,
			ls_Param_In MemoryAllocater* arena)  const override;

		virtual void commit() override {}

		virtual std::string strOut() const override;

		/*
			连接两条路径并计算对应的throughput
		*/
		virtual Spectrum connectPath(
			ls_Param_In ScenePtr scene,
			ls_Param_In SamplerPtr sampler,
			ls_Param_In CameraPtr camera,
			ls_Param_In const Path& lightPath,
			ls_Param_In const Path& cameraPath,
			ls_Param_In s32 s, // 光源路径 顶点数
			ls_Param_In s32 t, // 相机路径 数点数
			ls_Param_In ls_Param_Out CameraSample* cameraSample);


		/*
			生成 MIS 
			Brute Force
		*/
		virtual f32 BDPTMIS(
			ls_Param_In const Path& lightPath,
			ls_Param_In const Path& cameraPath,
			ls_Param_In s32 s, //光源路径 顶点数
			ls_Param_In s32 t //相机路径 顶点数
			);

		/*
			生成 MIS
		*/
		virtual f32 BDPTMISEfficiency(
			ls_Param_In const Path& lightPath,
			ls_Param_In const Path& cameraPath,
			ls_Param_In s32 s,
			ls_Param_In s32 t);
	protected:
		BDPT(ParamSet& paramSet);
	private:
		s32			mPathMaxDepth;
	};
}