#pragma once


#include<math/vector.h>
#include<DirectXMath.h>
namespace ls
{
	class Mat4x4
	{
	public:

		//构造函数
		Mat4x4(){ memset(&m[0][0], 0, sizeof(f32) * 16); }
		Mat4x4(f32 m00, f32 m01, f32 m02, f32 m03,
			f32 m10, f32 m11, f32 m12, f32 m13,
			f32 m20, f32 m21, f32 m22, f32 m23,
			f32 m30, f32 m31, f32 m32, f32 m33)
		{
			m[0][0] = m00, m[0][1] = m01, m[0][2] = m02, m[0][3] = m03;
			m[1][0] = m10, m[1][1] = m11, m[1][2] = m12, m[1][3] = m13;
			m[2][0] = m20, m[2][1] = m21, m[2][2] = m22, m[2][3] = m23;
			m[3][0] = m30, m[3][1] = m31, m[3][2] = m32, m[3][3] = m33;

		}
		Mat4x4(std::initializer_list<f32> il)
		{

			assert(il.size() == 16);
			auto p = il.begin();
			m[0][0] = *(p++), m[0][1] = *(p++), m[0][2] = *(p++), m[0][3] = *(p++);
			m[1][0] = *(p++), m[1][1] = *(p++), m[1][2] = *(p++), m[1][3] = *(p++);
			m[2][0] = *(p++), m[2][1] = *(p++), m[2][2] = *(p++), m[2][3] = *(p++);
			m[3][0] = *(p++), m[3][1] = *(p++), m[3][2] = *(p++), m[3][3] = *(p++);

		}
		Mat4x4(const Vec4& row0,
			const Vec4& row1,
			const Vec4& row2,
			const Vec4& row3)
		{
			memcpy(&m[0][0], &row0, sizeof(f32) * 4);
			memcpy(&m[1][0], &row1, sizeof(f32) * 4);
			memcpy(&m[2][0], &row2, sizeof(f32) * 4);
			memcpy(&m[3][0], &row3, sizeof(f32) * 4);

		}
		explicit Mat4x4(f32 mat[4][4])
		{
			memcpy(m, mat, sizeof(f32) * 16);
		}
		Mat4x4(const Mat4x4& mat)
		{
			memcpy(&m[0][0], &mat.m[0][0], sizeof(f32) * 16);
		}

		Mat4x4& operator=(const Mat4x4& mat)
		{
			memcpy(&m[0][0], &mat.m[0][0], sizeof(f32) * 16);
			return *this;
		}

		ls_ForceInline f32& operator()(u32 x, u32 y)
		{
			assert(x >= 0 && x < 4);
			assert(y >= 0 && y < 4);

			return m[x][y];
		}
		ls_ForceInline f32 operator()(u32 x, u32 y) const
		{
			assert(x >= 0 && x < 4);
			assert(y >= 0 && y < 4);

			return m[x][y];
		}

		ls_ForceInline f32& operator()(u32 i)
		{
			assert(i >= 0 && i < 16);

			return m[i / 4][i % 4];
		}
		ls_ForceInline f32 operator()(u32 i) const
		{
			assert(i >= 0 && i < 16);

			return m[i / 4][i % 4];
		}

		Mat4x4 operator+(const Mat4x4& mat) const
		{
			Mat4x4 r;
			for (u32 i = 0; i < 16; ++i)
				r(i) = m[i / 4][i % 4] + mat(i);

			return r;
		}
		Mat4x4 operator-(const Mat4x4& mat) const
		{
			Mat4x4 r;
			for (u32 i = 0; i < 16; ++i)
				r(i) = m[i / 4][i % 4] - mat(i);

			return r;
		}
		Mat4x4 operator*(const Mat4x4& mat) const
		{

			auto xm = Mat2XMMATRIX(m);
			auto xmat = Mat2XMMATRIX(&mat);
			auto xr = xm * xmat;
			Mat4x4 r;
			XMMATRIX2Mat(&r, xr);
			return r;
		}
		Mat4x4 transpose()					const
		{		
			auto xm = DirectX::XMLoadFloat4x4((const DirectX::XMFLOAT4X4*)(m));
			auto xmt=DirectX::XMMatrixTranspose(xm);
			Mat4x4 r;
			DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)(&r),xmt);
			return r;
		}
		Mat4x4 inverse()					const
		{

			auto xm= DirectX::XMLoadFloat4x4((const DirectX::XMFLOAT4X4*)(m));
			DirectX::XMVECTOR det;
			auto xmi = DirectX::XMMatrixInverse(&det, xm);
			Mat4x4 r;
			DirectX::XMStoreFloat4x4((DirectX::XMFLOAT4X4*)(&r), xmi);
			return r;
		}


		friend Vec4	operator*(const Vec4& v, const Mat4x4& m)
		{

			Vec4 r;
			r.x = v.x*m(0, 0) + v.y*m(1, 0) + v.z*m(2, 0) + v.w*m(3, 0);
			r.y = v.x*m(0, 1) + v.y*m(1, 1) + v.z*m(2, 1) + v.w*m(3, 1);
			r.z = v.x*m(0, 2) + v.y*m(1, 2) + v.z*m(2, 2) + v.w*m(3, 2);
			r.w = v.x*m(0, 3) + v.y*m(1, 3) + v.z*m(2, 3) + v.w*m(3, 3);

			return r;

		}

		

		std::string toString()
		{
			std::ostringstream ss;
			ss << "Matrix: " << std::endl;
			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					ss << m[i][j] << " ";
				}
				ss << std::endl;
			}
			return ss.str();
		}

		//	static Mat4x4						Identity;




		//缩放矩阵
		static Mat4x4  Mat4x4Scale(f32 x, f32 y, f32 z);
		static Mat4x4  Mat4x4Scale(const Vec3& v);
		//平移矩阵
		static Mat4x4  Mat4x4Translate(f32 x, f32 y, f32 z);
		static Mat4x4  Mat4x4Translate(const Vec3& v);
		//旋转矩阵（弧度制，绕轴旋转）
		static Mat4x4  Mat4x4Rotate(f32 radian, const Vec3& v);
		static Mat4x4  Mat4x4Rotate(f32 radian, f32 x, f32 y, f32 z);
		static Mat4x4  Mat4x4RotateX(f32 radian);
		static Mat4x4  Mat4x4RotateY(f32 radian);
		static Mat4x4  Mat4x4RotateZ(f32 radian);

		private:
			//16位字节对齐
			//m[4][4]按照行存储方式存放数据
			f32 m[4][4];
	};



}





