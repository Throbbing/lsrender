#pragma once

#include<config/config.h>
#include<config/declaration.h>
#include<function/log.h>
#include<math/vector.h>
namespace ls
{
	class CameraSample
	{
	public:
		CameraSample() { Unimplement; }
		~CameraSample() {}
	};

	class Sampler
	{
	public:
		Sampler() {}
		virtual ~Sampler() {}

		virtual f32 next1D() = 0;
		virtual Point2 next2D() = 0;
		
	};
}