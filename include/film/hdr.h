#pragma once
#include<film\film.h>
#include<vector>
#include<array>

namespace ls
{
	class HDRFilm :public ls::Film
	{
		friend ResourceManager;
	public:
		HDRFilm() {}
		virtual ~HDRFilm() {}

		virtual void commit() override;
		virtual std::string strOut() const override;

		virtual void addPixel(const Spectrum& color,
			float xpos, float ypos) override;

		virtual void flush()	override;

		virtual TexturePtr convert2Texture() const override;

	protected:
		HDRFilm(ParamSet& paramSet);
	protected:
		std::vector<Pixel>				mRenderBuffer;
		

	};
}