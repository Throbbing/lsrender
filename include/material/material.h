#pragma once
#include<config/config.h>
#include<config/declaration.h>

namespace ls
{
	class Material
	{
		friend Scene;
	public:
		Material() {}
		virtual ~Material() {}

		virtual ScatteringFunction* getSurfaceScattering() { return nullptr; }
		virtual ScatteringFunction* getMediumScattering()  { return nullptr; }


	};
}