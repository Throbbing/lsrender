#include<resource\paramSet.h>


namespace ls
{
	f32 ParamSet::queryf32(const std::string & name, f32 value) 
	{
		if (f32s.find(name) == f32s.end())
			return value;

		return f32s[name];
	}
	s32 ParamSet::querys32(const std::string & name, s32 value)
	{
		if (s32s.find(name) == s32s.end())
			return value;

		return s32s[name];
	}
	bool ParamSet::querybool(const std::string & name, bool value) 
	{
		if (bools.find(name) == bools.end())
			return value;

		return bools[name];
	}
	std::string ParamSet::queryString(const std::string & name, std::string value)
	{
		if (strings.find(name) == strings.end())
			return value;

		return strings[name];
	}
	Vec2 ParamSet::queryVec2(const std::string & name, Vec2 value) 
	{
		if (Vec2s.find(name) == Vec2s.end())
			return value;

		return Vec2s[name];
	}
	Vec3 ParamSet::queryVec3(const std::string & name, Vec3 value) 
	{
		if (Vec3s.find(name) == Vec3s.end())
			return value;
		return Vec3s[name];
	}
	Vec4 ParamSet::queryVec4(const std::string & name, Vec4 value) 
	{
		if (Vec4s.find(name) == Vec4s.end())
			return value;
		return Vec4s[name];
	}
	std::map<std::string, std::string> ParamSet::getAllRefs()
	{
		return refs;
	}
	Spectrum ParamSet::querySpectrum(const std::string & name, Spectrum value) 
	{
		if (Spectrums.find(name) == Spectrums.end())
			return value;

		return Spectrums[name];
	}
	Transform ParamSet::queryTransform(const std::string & name, Transform value)
	{
		if (Transforms.find(name) == Transforms.end())
			return value;

		return Transforms[name];
	}
	ParamSet ParamSet::queryParamSetByType(const std::string & type)
	{
		for (auto& p : childParamSets)
		{
			if (p.type == type)
				return p;
		}
		return ParamSet();
	}
	ParamSet ParamSet::queryParamSetByName(const std::string & name)
	{
		for (auto& p : childParamSets)
		{
			if (p.name == name)
				return p;
		}
		return ParamSet();
	}
	void ParamSet::addf32(const std::string & name, f32 value)
	{
		f32s[name] = value;
	}
	void ParamSet::adds32(const std::string & name, s32 value)
	{
		s32s[name] = value;
	}
	void ParamSet::addbool(const std::string & name, bool value)
	{
		bools[name] = value;
	}
	void ParamSet::addString(const std::string & name, std::string value)
	{
		strings[name] = value;
	}
	void ParamSet::addVec2(const std::string & name, Vec2 value)
	{
		Vec2s[name] = value;
	}
	void ParamSet::addVec3(const std::string & name, Vec3 value)
	{
		Vec3s[name] = value;
	}
	void ParamSet::addVec4(const std::string & name, Vec4 value)
	{
		Vec4s[name] = value;
	}
	void ParamSet::addSpectrum(const std::string & name, Spectrum value)
	{
		Spectrums[name] = value;
	}
	void ParamSet::addTransoform(const std::string & name, Transform value)
	{
		Transforms[name] = value;
	}
	void ParamSet::addParamSet(const std::string & name, ParamSet value)
	{
		childParamSets.push_back(value);
	}
	void ParamSet::addRef(const std::string & id, const std::string value)
	{
		refs[id] = value;
	}
}