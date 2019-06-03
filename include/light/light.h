#pragma once

#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
#include<spectrum/spectrum.h>
namespace ls
{
	enum LightType
	{
		ELight_Point = 0,
		ELight_Environment = 1,
		ELight_Area = 2
	};
	class Light:public Module
	{
		friend Scene;
	public:
		Light(const std::string& id = "Light"):Module(id){}
		virtual ~Light() {}


		virtual LightType getLightType() const = 0;

		virtual bool		isDelta() = 0;
		virtual Spectrum	getPower() = 0;



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
		//Sample�Ż��汾 ������������ײ
		//Ĭ�ϵ���sample(ray)
		virtual ls::Spectrum sample(const Ray& ray, const IntersectionRecord& its) {
			return sample(ray);
		}
		
		virtual f32			 pdf(const Ray& ray) { return 0.f; }
		virtual f32			 pdf(const Ray& ray, const IntersectionRecord& its)
		{
			return pdf(ray);
		}
		

		//Sample
		//���ؼ�¼���й�������Ҫ��Pdf
		//@refRef		[in]		��¼��
		//				[return]	pdf
		virtual f32 pdf(ls_Param_In const LightSampleRecord* refRec) = 0;


		virtual void commit() = 0;
		

	protected:
	};
}