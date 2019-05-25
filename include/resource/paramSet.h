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
		EParamSet_Texture,
		EParamSet_Medium,
		EParamSet_Phase,
		EParamSet_Invalid
	};
	class ParamSet
	{
		friend XMLParser;
	public:
		ParamSet(const std::string& strClass,const std::string& type,
			const std::string& name,
			const std::string& id):
			varClass(strClass),varType(type),varName(name),varID(id)
		{
			valid = true;
			
		}

		ParamSet() { valid = false; }
		bool isValid() const { return valid; }
		f32 queryf32(const std::string& name, f32 value = 0.f) ;
		s32 querys32(const std::string& name, s32 value = 0.f) ;
		bool querybool(const std::string& name, bool value  = false) ;
		std::string queryString(const std::string& name, std::string value = "") ;
		Vec2 queryVec2(const std::string& name, Vec2 value = Vec2()) ;
		Vec3 queryVec3(const std::string& name, Vec3 value = Vec3()) ;
		Vec4 queryVec4(const std::string& name, Vec4 value = Vec4()) ;

		std::string queryRefByName(const std::string& name, std::string value = "");
		std::map<std::string, std::string> getAllRefs() ;
		Spectrum querySpectrum(const std::string& name, Spectrum value = Spectrum()) ;
		Transform queryTransform(const std::string& name, Transform value = Transform()) ;
		
		std::vector<ParamSet> queryParamSetByClass(const std::string& varClass);
		std::vector<ParamSet> queryParamSetByType(const std::string& type) ;
		ParamSet queryParamSetByName(const std::string& name) ;

		void addf32(const std::string& name, f32 value) ;
		void adds32(const std::string& name, s32 value);
		void addbool(const std::string& name, bool value);
		void addString(const std::string& name, std::string value);
		void addVec2(const std::string& name, Vec2 value);
		void addVec3(const std::string& name, Vec3 value);
		void addVec4(const std::string& name, Vec4 value);
		void addSpectrum(const std::string& name, Spectrum value);
		void addTransform(const std::string& name, Transform value);
		void addParamSet(ParamSet value);
		void addRef(const std::string& name, const std::string id);


		std::string getClass() { return varClass; }
		std::string getType() { return varType; }
		std::string getName() { return varName; }
		std::string getID() { return varID; }

	private:
		bool				valid;
		std::string			varClass;
		std::string			varType;
		std::string			varName;
		std::string			varID;
		std::map<std::string, f32>			f32s;
		std::map<std::string, s32>			s32s;
		std::map<std::string, bool>			bools;
		std::map<std::string, std::string>  strings;
		std::map<std::string, Vec2>			Vec2s;
		std::map<std::string, Vec3>			Vec3s;
		std::map<std::string, Vec4>			Vec4s;
		std::map<std::string, Spectrum>		Spectrums;
		std::map<std::string, Transform>	Transforms;
		std::map<std::string, std::string>	refs;

		std::vector<ParamSet>	childParamSets;
	};

	
}