#pragma once

#include<config/config.h>
#include<config/declaration.h>
#include<config/module.h>
#include<config/lsPtr.h>
#include<function/log.h>
#include<math/vector.h>
namespace ls
{
	class CameraSample
	{
	public:
		CameraSample() { }
		~CameraSample() {}

		Point2 pos;
	};

	class Sampler:public Module
	{
	public:
		Sampler(const std::string& id = "sampler"):Module(id) {}
		virtual ~Sampler() {}

		virtual SamplerPtr copy() const = 0;
		

		virtual f32 next1D() = 0;
		virtual Point2 next2D() = 0;
		
	};
}