#include<config/lsPtr.h>
#include<config/common.h>
#include<camera/camera.h>
#include<algorithm/bdpt.h>
#include<record/record.h>
#include<function/stru.h>
#include<function/bidirStru.h>
#include<film/film.h>
#include<scene/scene.h>
#include<light/light.h>
#include<material/material.h>
#include<resource/xmlHelper.h>
#include<resource/resourceManager.h>
#include<sampler/sampler.h>
#include<spectrum/spectrum.h>
#include<scatter/scatter.h>


void ls::BDPT::render(ScenePtr scene,
	SceneRenderBlock * renderBlock,
	SamplerPtr sampler,
	CameraPtr camera,
	RNG & rng) const
{
	auto film = camera->getFilm();
	s32 spp = lsRender::sampleInfo.spp;

	for (s32 h = renderBlock->yStart; h < renderBlock->yEnd; ++h)
	{
		for (s32 w = renderBlock->xStart; w < renderBlock->xEnd; ++w)
		{
//			if (w != 256 || h != 256)
//				continue;
			for (s32 i = 0; i < spp; ++i)
			{	
				//if (w == 12 && h == 8 && i == 29)
				//{
				//	std::cout << "Fuck" << std::endl;
				//}
				Spectrum L(0.f);
				auto offset = sampler->next2D();
				Point2 initialPixelPos = Point2(w + offset.x, h + offset.y);
			
				CameraSample cs;
				cs.pos = initialPixelPos;
				/*
					���� ˫��·��
				*/
				auto cameraPath = BiDirPath::createPathFromCamera(
					sampler, scene, cs, camera, mPathMaxDepth);

				auto lightPath = BiDirPath::createPathFromLight(
					sampler, scene, mPathMaxDepth);

				auto cameraPathSize = cameraPath.size();
				auto lightPathSize = lightPath.size();

//				if (lightPathSize >= 3)
				{
					for (s32 s = 0; s <= lightPathSize; ++s)
					{
						for (s32 t = 1; t <= cameraPathSize; ++t)
						{
							cs.pos = initialPixelPos;
							auto result = connectPath(scene,
								sampler, camera,
								lightPath,
								cameraPath,
								s, t,
								&cs);

							if (t != 1)
								L += result;
							else
								film->addLightSample(result, cs.pos.x, cs.pos.y);
						}
					}
				}

				film->addPixel(L, initialPixelPos.x, initialPixelPos.y); 
			}

		}
	}
}

ls::RenderAlgorithmPtr ls::BDPT::copy() const
{
	ParamSet paramSet = ParamSet("renderAlgorithm", "bdpt", "bdpt", "bdpt");
	paramSet.adds32("maxDepth", mPathMaxDepth);

	return ResourceManager::createAlgorithm(paramSet);
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
	ls_Param_In CameraPtr camera,
	ls_Param_In const BiDirPath & lightPath, 
	ls_Param_In const BiDirPath & cameraPath, 
	ls_Param_In s32 s,
	ls_Param_In s32 t,
	ls_Param_In ls_Param_Out CameraSample * cameraSample) const
{
	/*
		O (n * n)
	*/
	if (t == 0)
		return Spectrum(0.f);

	Spectrum L(0.f);
	LightSampleRecord lightSampleRecord;
	f32 selectLightPdf = 0.f;
	ZeroMemory(&lightSampleRecord, sizeof(lightSampleRecord));
	
	/*
		case 1: s = 0 ��Դֱ�������·���Ķ˵�
	*/
	if (s == 0)
	{
		//ֱ�Ӽ����Դ����� Le
		auto& v = cameraPath[t - 1];
		if (v.getVertexType() != PathVertexType::EPathVertex_Light)
			return 0.f;

		L = v.throughput * v.pathVertexRecord.lightSampleRecord.le;
	}
	/*
		case 2: s = 1,ֱ�ӹ��ռ���
	*/
	else if (s == 1)
	{
		auto& cameraVertex = cameraPath[t - 1];

		if (!cameraVertex.isConnectable())
			return Spectrum(0.f);
		
		if (cameraVertex.getVertexType() != PathVertexType::EPathVertex_Surface)
			return 0.f;

		selectLightPdf = scene->sampleLight(sampler, &lightSampleRecord);
		auto light = lightSampleRecord.light;

		IntersectionRecord refIts;
		refIts.position = cameraVertex.position;
		refIts.ns = cameraVertex.ns;
		refIts.ng = cameraVertex.ns;

		light->sample(sampler, &refIts, &lightSampleRecord);

		/*
			���� BSDF ϵ��
		*/
		if (!RenderLib::visible(scene, refIts.position, lightSampleRecord.samplePosition))
			return Spectrum(0);

		ScatteringRecord sr;
		RenderLib::fillScatteringRecordForBSDFValueAndPdf(refIts.position,
			cameraVertex.ns,
			cameraVertex.wo,
			cameraVertex.wi,
			ETransport_Radiance,
			&sr);

		RenderLib::surfaceBSDFValueAndPdf(cameraVertex.material->getSurfaceScattering(),
			&sr);
		
		auto bsdfValue = sr.sampledValue * cameraVertex.material->scatteringFactor(sr,refIts);

		L = cameraVertex.throughput *
			bsdfValue *
			std::fabs(dot(cameraVertex.ns,-lightSampleRecord.sampleDirection)) * 
			lightSampleRecord.le
			/ lightSampleRecord.pdfW;
	}
	/*
		case 3: t = 1  ֻ��һ��������㣬������Ҫ���¼��� ���� λ��
	*/
	else if (t == 1)
	{
		auto& lightVertex = lightPath[s - 1];
		if (!lightVertex.isConnectable())
			return Spectrum(0);

		// �Ƿ������
		if (lightVertex.getVertexType() != PathVertexType::EPathVertex_Surface ||
			lightVertex.pathVertexRecord.surfaceSampleRecord.scatterFlag & EScattering_S)
			return 0.f;

		CameraSampleRecord cameraSampleRecord;
		IntersectionRecord refIts;
		refIts.position = lightVertex.position;
		refIts.ns = lightVertex.ns;
		refIts.ng = lightVertex.ns;
		
		camera->sample(sampler, refIts, cameraSample,&cameraSampleRecord);

		// �жϿɼ��� �� pdf ֵ
		if (!RenderLib::visible(scene,cameraSampleRecord.samplePosition, lightVertex.position) 
			|| cameraSampleRecord.pdfD == 0.f)
		{
			return 0.f;
		}

		/*
			Light BiDirPath Vertex �� Importance Transport ˳������

			wi ָ�� ���
			wo ָ�� ��Դ
		*/
		Vec3 wi = normalize(Vec3(cameraSampleRecord.samplePosition - refIts.position));
		Vec3 wo = lightVertex.wo;

		ScatteringRecord sr;
		RenderLib::fillScatteringRecordForBSDFValueAndPdf(refIts.position,
			lightVertex.ns,
			wo,
			wi,
			ETransport_Importance,
			&sr);

		RenderLib::surfaceBSDFValueAndPdf(lightVertex.material->getSurfaceScattering(),
			&sr);

		auto bsdfValue = sr.sampledValue * lightVertex.material->scatteringFactor(sr, refIts);

		
		L = lightVertex.throughput
			* bsdfValue *
			std::fabs(dot(lightVertex.ns, wi)) *
			cameraSampleRecord.we
			/ cameraSampleRecord.pdfD;
	}
	/*
		case 4: ��������� BDPT General ���ӷ�ʽ
	*/
	else
	{
		auto& cameraVertex = cameraPath[t - 1];
		auto& lightVertex = lightPath[s - 1];

		if (!cameraVertex.isConnectable() ||
			!lightVertex.isConnectable() ||
			!RenderLib::visible(scene, cameraVertex.position, lightVertex.position))
			return Spectrum(0.f);

		/*
			���ӳ� Radiance Transport
			wi ָ���Դ
			wo ָ�����

			Light Vertex �� 
			wi ָ�����
			wo ָ���Դ
			���з�ת
		*/
		auto cameraWo = cameraVertex.wo;
		auto cameraWi = normalize(Vec3(lightVertex.position - cameraVertex.position));
		auto lightWo = lightVertex.wo;
		auto lightWi = -cameraWi;

		ScatteringRecord cameraSR;
		RenderLib::fillScatteringRecordForBSDFValueAndPdf(cameraVertex.position,
			cameraVertex.ns,
			cameraWo, cameraWi,
			ETransport_Radiance,
			&cameraSR);
		RenderLib::surfaceBSDFValueAndPdf(cameraVertex.material->getSurfaceScattering(), &cameraSR);
		auto cameraBSDFValue = cameraSR.sampledValue * cameraVertex.material->scatteringFactor(cameraSR, cameraVertex.getIntersection());

		ScatteringRecord lightSR;
		RenderLib::fillScatteringRecordForBSDFValueAndPdf(lightVertex.position,
			lightVertex.ns,
			lightWo, lightWi,
			ETransport_Importance,
			&lightSR);
		RenderLib::surfaceBSDFValueAndPdf(lightVertex.material->getSurfaceScattering(), &lightSR);
		auto lightBSDFValue = lightSR.sampledValue * lightVertex.material->scatteringFactor(lightSR, lightVertex.getIntersection());



		L = cameraVertex.throughput * cameraBSDFValue *
			RenderLib::G(cameraVertex.position, cameraVertex.ns, lightVertex.position, lightVertex.ns) *
			lightBSDFValue * lightVertex.throughput;

	}

	if (L.isBlack())
		return 0.f;

#if 0
	auto misWeight = BDPTMIS(lightPath,
		cameraPath,
		s, t);
#else
	auto misWeight = BDPTMISEfficiency(lightPath,
		lightSampleRecord,
		selectLightPdf,
		cameraPath,
		s, t);
#endif
	if (isnan(misWeight)) return 0.f;

	return L * misWeight;
}

f32 ls::BDPT::BDPTMIS(
	ls_Param_In const BiDirPath & lightPath,
	ls_Param_In const BiDirPath & cameraPath, 
	ls_Param_In s32 s,
	ls_Param_In s32 t) const
{
	/*
		O (n * n)
	*/
	if (s + t == 2 || s == 0 || t == 1) return 1.f;

	/*
		P_(s,t) 
	*/

	s32 k = s + t - 1;
	
	f32 pst = 1.f;
	f32 pdfSum = 0.f;

#if 0
	// i ���� Light BiDirPath �� �� �������

	// case 1: i <= s
	for (s32 i = 1; i <= s; ++i)
	{
		f32 pdf = 1.f;


		/*
			Importance Transport

			[0,i-1] ���� Light BiDirPath ��  
		*/
		for (s32 impIndex = 0; impIndex < i; ++impIndex)
		{
			pdf *= lightPath[impIndex].getImportancePdf();
		}

		/*
			Radiance Transport

			[i,s - 1]   �� Light  BiDirPath ��
			[s,s + t]   �� Camera BiDirPath ��
			
			���ڴ����������෴�ģ�����ȡ Camera BiDirPath �Ķ���������Ҫ��
		*/
		for (s32 radIndex = i; radIndex < s; ++radIndex)
		{
			pdf *= lightPath[radIndex].getRadiancePdf();
		}
		for (s32 radIndex = t - 1; radIndex >= 0; radIndex--)
		{
			pdf *= cameraPath[radIndex].getRadiancePdf();
		}

		pdfSum += pdf;

		if (i == s)
			pst = pdf;
	}

	//case 2 : i > s
	for (int i = s + 1; i <= s + t; ++i)
	{
		f32 pdf = 1.f;
		/*
			Importance Transport

			[0,s-1] �� Light BiDirPath ��
		*/
		for (s32 impIndex = 0; impIndex < s; ++impIndex)
		{
			pdf *= lightPath[impIndex].getImportancePdf();
		}

		/*
			Importance Transport

			[s, i] �� Camera BiDirPath�� 
		*/
		for(s32 impIndex = t - 1;impIndex >= )
	}
	
#endif

	std::vector<const PathVertex*> vertices;

	for (s32 i = 0; i < s; ++i)
		vertices.push_back(&lightPath[i]);
	for (s32 i = t - 1; i >= 0; --i)
		vertices.push_back(&cameraPath[i]);

	/*
		���ǵ��������⣬���ȼ��� P_{s,t}

		Weight = 1.f / (\sum{P_i / P_{s,t}})
	*/
	for (s32 impIndex = 0; impIndex < s; ++impIndex)
	{
		pst *= vertices[impIndex]->getImportancePdf();
	}
	/*
		Radiance Transport
	*/
	for (s32 radIndex = s; radIndex < s + t + 1; ++radIndex)
	{
		pst *= vertices[radIndex]->getRadiancePdf();
	}

	/*
		i ���� ���Ӳ����� Light BiDirPath �Ķ�����
	*/
	for (s32 i = 1; i < s + t + 1 ; ++i)
	{
		// P_{s,t} �Ѿ����㣬����Ҫ�ظ�����
		if (i == s)
		{
			pdfSum += 1.f;
			continue;
		}


		f32 pdf = 1.f;

		/*
			Importance Transport
		*/
		for (s32 impIndex = 0; impIndex < i; ++impIndex)
		{
			pdf *= vertices[impIndex]->getImportancePdf();
		}
		/*
			Radiance Transport
		*/
		for (s32 radIndex = i; radIndex < s + t + 1; ++radIndex)
		{
			pdf *= vertices[radIndex]->getRadiancePdf();
		}

		pdfSum += (pdf / pst);


	}

	if (pdfSum == 0.f || isnan(pdfSum) || isnan(pst))
		return 0.f;

	return 1.f / pdfSum;

}

f32 ls::BDPT::BDPTMISEfficiency(
	ls_Param_In const BiDirPath & lightPath, 
	ls_Param_In const LightSampleRecord& lightSampleRecord, // ֻ�� s = 1 ʱ���� Record �Żᱻʹ��
	ls_Param_In f32	  selectLightPdf,						// ֻ�� s = 1 ʱ���� Record �Żᱻʹ��
	ls_Param_In const BiDirPath & cameraPath, 
	ls_Param_In s32 s, ls_Param_In s32 t) const
{
	///*
	//	��ʱ�Ȳ����� s = 0
	//*/
	//if (s == 0)
	//	return 0;

	// ���� ���ӵ�� BSDF �� pdf

	BiDirPath mergePath(EPath_Light);
	if (s == 1)
	{
		ls_Assert(selectLightPdf != f32(0));
		mergePath = BiDirPath::mergePath(cameraPath, lightSampleRecord, selectLightPdf, t, EPath_Light);
	}
	else
	{
		mergePath = BiDirPath::mergePath(cameraPath, lightPath, t, s, EPath_Light);
	}

	auto correctValue = [](f32 v)->f32 { return v == 0.f ? 1.f : v; };

	/*
		�� i = s ��ʼ���м���������ɢ
		pdfWeights ���� debug
		pdfWeights[i] : P_{i} / P_s
	*/
	std::vector<f32> pdfWeights(s + t + 2,1.f);
	
	f32 misWeight = 0.f;
	
	// i �����Դ��·���ĸ���
	
	/*
		��Դ��·������

		i < s
		P_i / P_{i + 1} = P(x_i)_ra / P(x_i)_im
	*/
	for (s32 i = s - 1; i >= 0; --i)
	{		
		pdfWeights[i] = correctValue(mergePath[i].getRadiancePdf()) / correctValue(mergePath[i].getImportancePdf()) * pdfWeights[i + 1];
		
		bool isPossible = false;

		if (i == 0)
		{
			isPossible = (mergePath[0].getVertexType() == EPathVertex_Light &&
				!mergePath[0].pathVertexRecord.lightSampleRecord.light->isDelta());
		}
		else
		{
			isPossible = PathVertex::connectable(mergePath[i - 1], mergePath[i]);
		}

		if(isPossible)
			misWeight += pdfWeights[i];
	}

	/*
		�����·������

		i > s
		P_i / P_{i - 1} = P(x_{i - 1})_im / P(x_{i - 1})_ra
	*/
	for (s32 i = s + 1; i < s + t;++i)
	{
		
		pdfWeights[i] = correctValue(mergePath[i - 1].getImportancePdf()) / correctValue(mergePath[i - 1].getRadiancePdf()) * pdfWeights[i - 1];

		if (PathVertex::connectable(mergePath[i - 1], mergePath[i]))
			misWeight += pdfWeights[i];
	}

	ls_Assert(misWeight > 0.f);
	return 1.f / (1.f + misWeight);

}

ls::BDPT::BDPT(ParamSet & paramSet)
{
	mPathMaxDepth = paramSet.querys32("maxDepth", 1);
}
