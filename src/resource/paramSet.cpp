#include<resource\paramSet.h>
#include<math\vector.h>
#include<math\transform.h>
#include<spectrum\spectrum.h>

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
}