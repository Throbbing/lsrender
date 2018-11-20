#pragma once

#include<config/config.h>
#include<config/declaration.h>
#include<math/matrix.h>

namespace ls
{
	
	const Mat4x4 Identity = { 1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f };
	class Transform
	{
	public:
		Transform() { matrix  = Identity; }
		Transform(const Mat4x4& mat)
		{
			matrix = mat;

		}
		explicit Transform(f32 m[4][4])
		{
			matrix = Mat4x4(m);

		}
		Transform(std::initializer_list<f32> il)
		{
			matrix = Mat4x4(il);

		}
		Mat4x4 getMat()
		{
			return matrix;
		}

		Transform inverse() const
		{
			return Transform(matrix.inverse());
		}

		Vec3 operator()(const Vec3& v) const
		{
#if defined(usingDirectXMath)

			auto xmv = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(&v));
			auto xmm = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)(&matrix));
			auto xmr = DirectX::XMVector3TransformNormal(xmv, xmm);
			Vec3 r;
			DirectX::XMStoreFloat3((DirectX::XMFLOAT3*)(&r), xmr);
			return r;
#else
			Vec3 r;
			r.x = v.x*matrix(0, 0) + v.y*matrix(1, 0) + v.z*matrix(2, 0);
			r.y = v.x*matrix(0, 1) + v.y*matrix(1, 1) + v.z*matrix(2, 1);
			r.z = v.x*matrix(0, 2) + v.y*matrix(1, 2) + v.z*matrix(2, 2);

			return r;
#endif
		}
		Point3 operator()(const Point3& p) const
		{

			auto xmp = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(&p));
			auto xmm = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)(&matrix));
			auto xmr = DirectX::XMVector3TransformCoord(xmp, xmm);
			Point3 r;
			DirectX::XMStoreFloat3((DirectX::XMFLOAT3*)(&r), xmr);
			return r;

		}
		Normal operator()(const Normal& n) const 
		{
			/*
			Normal r; 
			r.x = inv(0, 0)*n.x + inv(0, 1)*n.y + inv(0, 2)*n.z;
			r.y = inv(1, 0)*n.x + inv(1, 1)*n.y + inv(1, 2)*n.z;
			r.z = inv(2, 0)*n.x + inv(2, 1)*n.y + inv(2, 2)*n.z;
			return r;
			*/
#if defined(usingDirectXMath)
			auto xmn = DirectX::XMLoadFloat3((DirectX::XMFLOAT3*)(&n));
			auto xmm = DirectX::XMLoadFloat4x4((DirectX::XMFLOAT4X4*)(&matrix));
			auto xmr = DirectX::XMVector3TransformNormal(xmn, xmm);
			Normal r;
			DirectX::XMStoreFloat3((DirectX::XMFLOAT3*)(&r), xmr);
			return r;
#else

			Normal r;
			r.x = n.x*matrix(0, 0) + n.y*matrix(1, 0) + n.z*matrix(2, 0);
			r.y = n.x*matrix(0, 1) + n.y*matrix(1, 1) + n.z*matrix(2, 1);
			r.z = n.x*matrix(0, 2) + n.y*matrix(1, 2) + n.z*matrix(2, 2);

			return r;
#endif
		}
		Ray operator()(const Ray& r) const;
		DifferentialRay operator()(const DifferentialRay& r) const;
		AABB operator()(const AABB& bbox) const;


		//static method
		//Ëõ·Å¾ØÕó
		static Mat4x4  Mat4x4Scale(f32 x, f32 y, f32 z);
		static Mat4x4  Mat4x4Scale(const Vec3& v);
		//Æ½ÒÆ¾ØÕó
		static Mat4x4  Mat4x4Translate(f32 x, f32 y, f32 z);
		static Mat4x4  Mat4x4Translate(const Vec3& v);
		//Ðý×ª¾ØÕó£¨»¡¶ÈÖÆ£¬ÈÆÖáÐý×ª£©
		static Mat4x4  Mat4x4Rotate(f32 radian, const Vec3& v);
		static Mat4x4  Mat4x4Rotate(f32 radian, f32 x, f32 y, f32 z);
		static Mat4x4  Mat4x4RotateX(f32 radian);
		static Mat4x4  Mat4x4RotateY(f32 radian);
		static Mat4x4  Mat4x4RotateZ(f32 radian);

		//Ïà»ú¾ØÕó
		static Mat4x4   Mat4x4Camera(const Vec3& look, const Vec3& up,
			const Point3& p);
		
		static Mat4x4 Mat4x4Perspective(f32 fov, f32 near, f32 far);
		
	private:
		Mat4x4 matrix;
//		Mat4x4 inv;
	};
}