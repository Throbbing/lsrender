#pragma once
#include<config/declaration.h>


namespace ls
{
	//Module
	using ModulePtr				= Module*;
	//About object
	using LightPtr				= Light*;
	using MeshPtr				= Mesh*;
	using MaterialPtr			= Material*;
	//About light transport
	using ScatteringFunctionPtr = ScatteringFunction*;
	using MediumPtr				= Medium*;
	using PhasePtr				= Phase*;
	using TexturePtr			= Texture*;
	//About Algorithm
	using RenderAlgorithmPtr	= RenderAlgorithm*;
	//About resource manage
	using MemoryAllocaterPtr	= MemoryAllocater*;
	using ResourceManagerPtr	= ResourceManager*;
	//About Random
	using RNGPtr				= RNG*;
	//About Sample
	using SamplerPtr			= Sampler*;
	class CameraSample;
	//About Camera and film
	using CameraPtr				= Camera*;
	using FilmPtr				= Film*;
	//About Scene
	using ScenePtr				= Scene*;
	//About RealtimeRenderer
	using RealtimeRendererPtr	= RealtimeRenderer*;
	using RealtimeRenderCommandPtr = RealtimeRenderCommand * ;
	using CanvasPtr				= Canvas * ;

	//About Timer
	using TimerPtr				= Timer*;

	//About Thread
	using ThreadPtr				= Thread* ;
	using ThreadTaskPtr			= ThreadTask*;
//	using ThreadEventPtr		= ThreadEvent*;
	using QueuedThreadPoolPtr	= QueuedThreadPool*;
	


	template<typename T>
	__forceinline static void ReleaselsPtr(T& p)
	{
		delete p;
		p = nullptr;
	}

	template<typename T>
	__forceinline static T* NewPtr()
	{
		return new T;
	}
}