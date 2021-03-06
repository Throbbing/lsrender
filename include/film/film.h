#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
#include<spectrum/spectrum.h>
namespace ls
{

	struct Pixel
	{
		u32 x, y;
		Spectrum color = Spectrum(0.f);
		float weight = 0.f;
	};
	class Film:public Module
	{
	public:
		Film(const std::string& id = "film"):Module(id){}
		virtual ~Film() {}


		virtual FilmPtr copy() const = 0;
		virtual void merge(const std::vector<FilmPtr>& films) = 0;
		virtual void setResolution(s32 w, s32 h) { mWidth = w; mHeight = h; }

		virtual s32 getWidth() { return mWidth; }
		virtual s32 getHeight() { return mHeight; }

		//commit changes into film
		virtual void commit() = 0;
		virtual void addPixel(const Spectrum& color,
			float xpos,float ypos) = 0;
		virtual void addLightSample(const Spectrum& color,
			float xpos, float ypos) = 0;

		virtual void flush() = 0;
		virtual TexturePtr convert2Texture() const = 0;


	protected:
		s32				mWidth = -1;
		s32				mHeight = -1;

		

	};
}