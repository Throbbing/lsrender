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
	struct lsMath
	{
		//constant value
		
		
		template<typename T>
		static T Square(T v) { return v*v; }



		constexpr static f32 Infinity = std::numeric_limits<f32>::max();
		constexpr static f32 Epsilon = 1e-6;
		constexpr static f32 PI = 3.1415926f;
		constexpr static f32 PI_2 = 6.2831852f;

		static f32 expf(f32 v) { return std::expf(v); }
		static f32 logf(f32 v) { return std::logf(v); }
		static f32 clamp(f32 v, f32 low, f32 high)
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

		static f32 distance(Point3 p1, Point3 p2);
		

#define Mat2XMMATRIX(m) DirectX::XMLoadFloat4x4((const DirectX::XMFLOAT4X4*)(m))
#define XMMATRIX2Mat(dest,xm) DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)dest,xm)

			

	};

	
	
	


}


