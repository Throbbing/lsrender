
#pragma  once
#include<config/config.h>
#include<config/declaration.h>
#include<math/math.h>
#include<math/vector.h>
namespace ls
{
	
	//¶ÀÁ¢×ø±êÏµ
	class Frame
	{
	public:
		Frame(const Vec3& xx, const Vec3& yy, const Vec3& zz) :
			X(xx), Y(yy), Z(zz){}
		Frame(Normal n);
		Frame(){}
		~Frame(){}
		
		
		Vec3 toLocal(const Vec3& v) const
		{
			return Vec3(dot(v, X), dot(v, Y), dot(v, Z));
		}
		Normal toLocal(const Normal& n) const 
		{
			return Normal(dot(n, X), dot(n, Y), dot(n, Z));
		}
		Vec3 toWorld(const Vec3& v) const 
		{
			return v.x*X + v.y*Y + v.z*Z;
		}
		Normal toWorld(const Normal& n)
		{
			return Normal(n.x*X + n.y*Y + n.z*Z);
		}

		static f32 cosTheta(const Vec3& v)
		{
			return v.z;
		}

		static f32 absCosTheta(const Vec3& v)
		{
			return std::fabs(v.z);
		}

		static f32 sinTheta2(const Vec3& v)
		{
			f32 cosa = v.z;
			return std::max(0.f, 1.f - cosa*cosa);
		}

		static f32 sinTheta(const Vec3& v)
		{
			return std::sqrtf(sinTheta2(v));
		}

		static f32 cosPhi(const Vec3& v)
		{
			f32 sinT = sinTheta(v);
			if (sinT == 0.f)return 1.f;
			return lsMath::clamp(v.x / sinT, -1.f, 1.f);
		}

		static f32 sinPhi(const Vec3& v)
		{
			f32 sinT = sinTheta(v);
			if (sinT == 0.f) return 1.f;
			return lsMath::clamp(v.y / sinT, -1.f, 1.f);
		}

		static f32 tanTheta(const Vec3& v)
		{
			f32 temp = 1.f - v.z * v.z;
			if (temp <= 0.f || v.z == 0.f)
				return 0.f;

			f32 sinTheta = std::sqrtf(temp);
			return sinTheta / v.z;
		}

		static f32 tanTheta2(const Vec3& v)
		{
			f32 temp = 1.f - v.z *v.z;
			if (temp <= 0.f || v.z == 0.f)
				return 0.f;

			return temp / (v.z * v.z);
		}

		static f32 sphericalTheta(const Vec3& v)
		{
			return std::acosf(lsMath::clamp(v.z, -1.f, 1.f));
		}

		static f32 sphericalPhi(const Vec3& v)
		{
			f32 phi = std::atan2f(v.y, v.x);
			return phi < 0.f ? phi + 2 * lsMath::PI: phi;
		}

		static bool hemisphere(const Vec3& v0,
			const Vec3& v1)
		{
			return v0.z * v1.z > 0.f;
		}

		static void constructFrame(ls_Param_In const Vec3& v1,
			ls_Param_Out Vec3* v2,
			ls_Param_Out Vec3* v3)
		{
			if (std::fabsf(v1.x) > std::fabsf(v1.y)) {
				f32 invLen = 1.f / std::sqrtf(v1.x*v1.x + v1.z*v1.z);
				*v2 = Vec3(-v1.z * invLen, 0.f, v1.x * invLen);
			}
			else {
				f32 invLen = 1.f / std::sqrtf(v1.y*v1.y + v1.z*v1.z);
				*v2 = Vec3(0.f, v1.z * invLen, -v1.y * invLen);
			}
			*v3 = cross(v1, *v2);
		}

		Vec3 X, Y, Z;
		
	};
}