#pragma once
#include<texture/texture.h>
#include<spectrum/spectrum.h>
#include<record/record.h>
namespace ls
{
	class ConstantTexture :public Texture
	{
	public:
		ConstantTexture(const Spectrum& spectrum) :Texture(ETexConstant)
		{ 
			applySpectrum(spectrum);
		}
		virtual ~ConstantTexture() {}
		void applySpectrum(const Spectrum& spectrum) { mColor = spectrum; }
		

		virtual void commit() override;
		virtual Spectrum fetch(ls_Param_In const IntersectionRecord* rec) override;


	protected:
		Spectrum			mColor;
	};
}