#pragma once
#include<config/declaration.h>


namespace ls
{
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
	//About Preview
	using PreviewerPtr			= Previewer*;


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