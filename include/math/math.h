#pragma once
#include<config/config.h>
#include<cmath>
#include<limits>
#include<DirectXMath.h>
#include<algorithm>

namespace ls
{
	struct Point3;
	struct Vec3;
	struct Normal;

	struct lsMath
	{
		//constant value
		
		
		template<typename T>
		static T Square(T v) { return v*v; }



		constexpr static f32 Infinity = 3.402823466e+38F;
		constexpr static f32 Epsilon = 1e-4;
		constexpr static f32 PI = 3.141592653589f;
		constexpr static f32 PI_2 = 6.283185307178f;
		constexpr static f32 PI_4 = 12.566370614356f;
		constexpr static f32 InvPi = 0.31830988618379067154;
		constexpr static f32 Inv2Pi = 0.15915494309189533577;
		constexpr static f32 Inv4Pi = 0.07957747154594766788;
		constexpr static f32 Inv180 = 0.00555555555555555556;

		static f32 expf(f32 v) { return std::expf(v); }
		static f32 logf(f32 v) { return std::logf(v); }
		static f32 clamp(f32 v, f32 low, f32 high)
		{
			if (v < low) return low;
			else if (v > high) return high;
			else return v;
		}
		static s32 clamp(s32 v, s32 low, s32 high)
		{
			if (v < low) return low;
			else if (v > high) return high;
			else return v;
		}
		static f32 absCos(f32 radian)
		{
			return	std::fabsf(std::cosf(radian));
		}

		static bool closeZero(f32 v) 
		{
			if (std::fabs(v) < lsMath::Epsilon)
				return true;

			return false;
		}

		static void dir2SphereCoordinate(ls_Param_In const Vec3& v,
			ls_Param_Out f32* theta,
			ls_Param_Out f32* phi);

		

		static f32 distance(Point3 p1, Point3 p2);

		static f32 radian2Degree(float radian);
		static f32 degree2Radian(float degree);

		static bool refract(ls_Param_In const Normal& n,
			ls_Param_In const Vec3& wi,
			ls_Param_In f32 etaI,
			ls_Param_In f32 etaT,
			ls_Param_Out Vec3* wt);

#define Mat2XMMATRIX(m) DirectX::XMLoadFloat4x4((const DirectX::XMFLOAT4X4*)(m))
#define XMMATRIX2Mat(dest,xm) DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)dest,xm)

			

	};

	
	
	


}


