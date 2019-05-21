#pragma once

#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
#include<spectrum/spectrum.h>
namespace ls
{
	class Light:public Module
	{
		friend Scene;
	public:
		Light(const std::string& id = "Light"):Module(id){}
		virtual ~Light() {}

		virtual bool		isDelta() = 0;
		virtual Spectrum	getPower() = 0;

		virtual void attachMesh(MeshPtr mesh) { mAttachedMesh = mesh; }
		virtual void detachMesh() { mAttachedMesh = nullptr; }


		//Sample
		//���ݹ�Դ�ֲ������õ��µķ����λ��
		//@sampler		[in]		������
		//@rec			[out]		��¼�� ��������õ��Ĺ���
		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_Out LightSampleRecord* rec) = 0;

		//Sample
		//����������ײ��͹�Դ�ֲ��õ��µķ����λ��
		//@sampler		[in]		������
		//@refRec		[in]		������ײ��¼��
		//@ref			[in]		��¼�� ��������õ��Ĺ���		
		virtual void sample(ls_Param_In SamplerPtr sampler,
			ls_Param_In const IntersectionRecord* refRec,
			ls_Param_Out LightSampleRecord* rec) = 0;


		//Sample
		//��Դ��ĳ�������Ϸ����������
		//@ray			[in]		��������
		//				[return]	��������
		virtual ls::Spectrum sample(const Ray& ray) { return ls::Spectrum(0); }
		virtual f32			 pdf(const Ray& ray) { return 0.f; }

		//Sample
		//���ؼ�¼���й�������Ҫ��Pdf
		//@refRef		[in]		��¼��
		//				[return]	pdf
		virtual f32 pdf(ls_Param_In const LightSampleRecord* refRec) = 0;


		virtual void commit() = 0;
		

	protected:
		MeshPtr					mAttachedMesh = nullptr;
	};
}