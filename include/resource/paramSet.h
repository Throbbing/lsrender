#pragma once
#include<config\config.h>
#include<config\declaration.h>

#include<map>
#include<string>
namespace ls
{
	
	class ParamSet
	{
	public:
		f32 queryf32(const std::string& name, f32 value = 0.f);
		s32 querys32(const std::string& name, s32 value = 0.f);
		bool querybool(const std::string& name, bool value  = false);
		Vec2 queryVec2(const std::string& name, Vec2 value = Vec2());
		Vec3 queryVec3(const std::string& name, Vec3 value = Vec3());
		Vec4 queryVec4(const std::string& name, Vec4 value = Vec4());
		Spectrum querySpectrum(const std::string& name, Spectrum value = Spectrum());
		Transform queryTransform(const std::string& name, Transform value = Transform());

	private:
		std::map<std::string, f32>  f32s;
		std::map<std::string, s32>	s32s;
		std::map<std::string, bool> bools;
		std::map<std::string, Vec2> Vec2s;
		std::map<std::string, Vec3> Vec3s;
		std::map<std::string, Vec4> Vec4s;
		std::map<std::string, Spectrum> Spectrums;
		std::map<std::string, Transform> Transforms;
	};

	
}