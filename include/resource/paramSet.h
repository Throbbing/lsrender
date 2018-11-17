#pragma once
#include<config\config.h>
#include<config\declaration.h>
#include<math\vector.h>
#include<math\transform.h>
#include<spectrum\spectrum.h>
#include<map>
#include<cstring>
namespace ls
{
	enum ParamSetType
	{
		EParamSet_Camera,
		EParamSet_Film,
		EParamSet_BSDF,
		EParamSet_Mesh,
		EParamSet_Light,
		EParamSet_Integrator,
		EParamSet_Medium,
		EParamSet_Phase,

	};
	class ParamSet
	{
	public:
		ParamSet(const std::string& strType,const std::string& n):
			type(type),name(n){}
		f32 queryf32(const std::string& name, f32 value = 0.f);
		s32 querys32(const std::string& name, s32 value = 0.f);
		bool querybool(const std::string& name, bool value  = false);
		Vec2 queryVec2(const std::string& name, Vec2 value = Vec2());
		Vec3 queryVec3(const std::string& name, Vec3 value = Vec3());
		Vec4 queryVec4(const std::string& name, Vec4 value = Vec4());
		Spectrum querySpectrum(const std::string& name, Spectrum value = Spectrum());
		Transform queryTransform(const std::string& name, Transform value = Transform());


		void addf32(const std::string& name, f32 value);
		void adds32(const std::string& name, s32 value);
		void addbool(const std::string& name, bool value);
		void addString(const std::string& name, std::string value);
		void addVec2(const std::string& name, Vec2 value);
		void addVec3(const std::string& name, Vec3 value);
		void addVec4(const std::string& name, Vec4 value);
		void addSpectrum(const std::string& name, Spectrum value);
		void addTransoform(const std::string& name, Transform value);
		void addParamSet(const std::string& name, ParamSet value);
		void addRef(const std::string& id, const std::string value);

//		ParamSetType getType() { return paramType; }
//		std::string getName() { return name; }

	private:
		std::string			type;
		std::string			name;
		std::map<std::string, f32>  f32s;
		std::map<std::string, s32>	s32s;
		std::map<std::string, bool> bools;
		std::map<std::string, Vec2> Vec2s;
		std::map<std::string, Vec3> Vec3s;
		std::map<std::string, Vec4> Vec4s;
		std::map<std::string, Spectrum> Spectrums;
		std::map<std::string, Transform> Transforms;

		std::vector<ParamSet>	childParamSets;
	};

	
}