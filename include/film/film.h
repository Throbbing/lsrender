#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<spectrum/spectrum.h>
namespace ls
{

	struct Pixel
	{
		f32			x, y;
		f32			time;
		Spectrum	radiance;
		

	};
	class Film
	{
	public:
		Film() {}
		virtual ~Film() {}


		virtual void addPixel(const Pixel& pixel) = 0;


	};
}