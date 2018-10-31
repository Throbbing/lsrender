#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<spectrum/spectrum.h>
namespace ls
{
	class Texture
	{
		friend Scene;
	public:
		Texture() {}
		virtual ~Texture() {}

		virtual Spectrum fetch(ls_Param_In const Record* rec) = 0;
	};
}