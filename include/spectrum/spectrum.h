#pragma once
#include<config/config.h>

#include<cmath>
#include<math/vector.h>
#include<config/declaration.h>

#define ARGB32(a,r,g,b)	(u32)((a<<24)+(r<<16)+(g<<8)+(b))

#define RGBA32(r,g,b,a)  (u32)((r<<24)+(g<<16)+(b<<8)+a)

#define RGB24(r,g,b)		((r<<16)+(g<<8)+b)

#define RGB16(r,g,b)		(((r&31)<<11)+((g&63)<<5)+((b&31)))

#define RGB16StoreARGB32(color)  (u32)((((color & 31) )<<3 )+ (((color & 0x7e0) >> 5) << 10) + (((color & 0xf800) >>11 )<<19 ))

#define ARGB32StoreRGB16(color)	(u16)((color&0xff)>>3+((color&0xff00)>>10)<<5+((color&0xff0000)>>19)<<11)


#define ARGB32StoreR8G8B8(color,r,g,b)  {((*r)=((color&0xff0000)>>16),(*g)=((color&0x00ff00)>>8),(*b)=((color&0x0000ff)));}

#define RGBA32StoreR8G8B8(color,r,g,b) {(*r)=((color&0xff000000)>>24),(*g)=((color&0xff0000)>>16),(*b)=((color&0xff00)>>8);}


namespace ls
{
	const f32 XtoRWeight[3] = { 3.240479f, -1.537150f, -0.498535f };
	const f32 YtoGWeight[3] = { -0.969256f, +1.875991f, +0.041556f };
	const f32 ZtoBWeight[3] = { 0.055648f, -0.204043f, +1.057311f };


	const f32 RtoXWeight[3] = { 0.412453f, 0.357580f, 0.180423f };
	const f32 GtoYWeight[3] = { 0.212671f, 0.715160f, 0.072169f };
	const f32 BtoZWeight[3] = { 0.019334f, 0.119193f, 0.950227f };


	inline f32 toSRGB(f32 v)
	{
		if (v < 0.0031308f)
			return 12.92f * v;

		return 1.055 * std::powf(v, 1.0 / 2.4) - 0.055;
	}
	//光谱
	//RGBSpectrum
	class Spectrum
	{
	public:
		Spectrum(f32 v = 0) { c[0] = c[1] = c[2] = v; }
		Spectrum(f32 r, f32 g, f32 b) { c[0] = r; c[1] = g; c[2] = b; }
		Spectrum(const Spectrum& s) { c[0] = s.c[0]; c[1] = s.c[1]; c[2] = s.c[2]; }
		explicit Spectrum(const Vec4& vec) { c[0] = vec.x; c[1] = vec.y; c[2] = vec.z; padding = 1.f; }
		Spectrum& operator=(const Spectrum& s)
		{
			c[0] = s.c[0];
			c[1] = s.c[1];
			c[2] = s.c[2];
			return *this;
		}
		~Spectrum() {}

		//光谱函数
		f32 luminance() const
		{
			return (GtoYWeight[0] * c[0] + GtoYWeight[1] * c[1] + GtoYWeight[2] * c[2]);
		}
		void toXYZ(f32 xyz[3]) const
		{
			xyz[0] = RtoXWeight[0] * c[0] + RtoXWeight[1] * c[1] + RtoXWeight[2] * c[2];
			xyz[1] = GtoYWeight[0] * c[0] + GtoYWeight[1] * c[1] + GtoYWeight[2] * c[2];
			xyz[2] = BtoZWeight[0] * c[0] + BtoZWeight[1] * c[1] + BtoZWeight[2] * c[2];

		}
		void toRGB(f32 rgb[3]) const
		{
			rgb[0] = c[0];
			rgb[1] = c[1];
			rgb[2] = c[2];
		}

		void toSRGB(f32 srgb[3]) const
		{
			srgb[0] = ls::toSRGB(c[0]);
			srgb[1] = ls::toSRGB(c[1]);
			srgb[2] = ls::toSRGB(c[2]);
		}

		u32 capRGBA() const
		{
			return RGBA32((u32)c[0], (u32)c[1], (u32)c[2], 0);
		}

		u32 capARGB() const
		{
			return ARGB32(0, (u32)c[0], (u32)c[1], (u32)c[2]);
		}

		//操作
		Spectrum operator+(const Spectrum& s) const
		{
			return Spectrum(c[0] + s.c[0], c[1] + s.c[1], c[2] + s.c[2]);
		}
		Spectrum& operator+=(const Spectrum& s)
		{
			c[0] += s.c[0];
			c[1] += s.c[1];
			c[2] += s.c[2];
			return *this;
		}
		Spectrum operator-(const Spectrum& s) const
		{
			return Spectrum(c[0] - s.c[0], c[1] - s.c[1], c[2] - s.c[2]);
		}
		Spectrum& operator-=(const Spectrum& s)
		{
			c[0] -= s.c[0];
			c[1] -= s.c[1];
			c[2] -= s.c[2];
			return *this;
		}
		Spectrum operator*(const Spectrum& s) const
		{
			return Spectrum(c[0] * s.c[0], c[1] * s.c[1], c[2] * s.c[2]);
		}
		Spectrum& operator*=(const Spectrum& s)
		{
			c[0] *= s.c[0];
			c[1] *= s.c[1];
			c[2] *= s.c[2];
			return *this;
		}
		Spectrum operator /(const Spectrum& s) const
		{
			return Spectrum(c[0] / s.c[0], c[1] / s.c[1], c[2] / s.c[2]);
		}
		Spectrum& operator/=(const Spectrum& s)
		{
			c[0] /= s.c[0];
			c[1] /= s.c[1];
			c[2] /= s.c[2];
			return *this;
		}

		Spectrum operator*(f32 s) const
		{
			return Spectrum(c[0] * s, c[1] * s, c[2] * s);
		}

		f32 operator[](u32 i) const
		{
			return c[i];
		}

		bool isBlack()
		{
			return c[0] == 0.f&&c[1] == 0.f&&c[2] == 0.f;
		}

		//调试函数，将Spectrum转换为可读的String
		std::string toString() const
		{
			std::ostringstream ss;
			ss << "Spectrum: " << c[0] << " " << c[1] << " " << c[2] << std::endl;

			return ss.str();
		}


		friend Spectrum operator * (f32 v, const Spectrum& s);



	private:
		//RGB
		f32 c[3];
		f32 padding = 1.f;

	};

	inline Spectrum lerp(const Spectrum& s0, const Spectrum& s1, f32 t)
	{
		return (1.f - t)*s0 + t*s1;
	}
	inline Spectrum operator*(f32 v, const ls::Spectrum& s)
	{
		return s*v;
	}

	inline Spectrum exp(const Spectrum& s)
	{
		return Spectrum(std::expf(s[0]), std::expf(s[1]), std::expf(s[2]));
	}

	inline Spectrum sqrt(const Spectrum& s)
	{
		return Spectrum(std::sqrtf(s[0]), std::sqrtf(s[1]), std::sqrtf(s[2]));
	}


}