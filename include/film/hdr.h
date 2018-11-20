#pragma once
#include<film\film.h>
#include<vector>
#include<array>

namespace ls
{
	class HDRFilm :public ls::Film
	{
	public:
		HDRFilm() {}
		virtual ~HDRFilm() {}

		virtual void commit() override;
		virtual void addPixel(const Spectrum& color,
			float xpos, float ypos) override;

		virtual void flush()	override;

	protected:
		std::vector<Pixel>				mRenderBuffer;

	};
}