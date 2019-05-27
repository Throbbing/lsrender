#include<math/math.h>
#include<math/vector.h>

constexpr f32 ls::lsMath::Infinity;
constexpr f32 ls::lsMath::Epsilon;

void ls::lsMath::dir2SphereCoordinate(ls_Param_In const Vec3 & v, ls_Param_Out f32 * theta, ls_Param_Out f32 * phi)
{
	f32 outPhi = std::atan2f(v.y, v.x);
	if (outPhi < 0.f) outPhi += lsMath::PI;
	
	*phi = outPhi;
	*theta = std::acosf(v.z);
}

f32 ls::lsMath::distance(Point3 p1, Point3 p2)
{
	return Vec3(p1-p2).length();
}

f32 ls::lsMath::radian2Degree(float radian)
{
	return radian * InvPi * 180.f;
}

f32 ls::lsMath::degree2Radian(float degree)
{
	return degree * Inv180 * PI;
}

bool ls::lsMath::refract(
	ls_Param_In const Normal& n,
	ls_Param_In const Vec3 & wi,
	ls_Param_In f32 etaI, 
	ls_Param_In f32 etaT, 
	ls_Param_Out Vec3 * wt)
{
	
	f32 eta = etaI / etaT;
	f32 cosThetaI = dot(n, wi);
	f32 sin2ThetaI = std::max(0.f, 1.f - cosThetaI * cosThetaI);
	f32 sin2ThetaT = eta * eta * sin2ThetaI;

	// 处理全反射
	if (sin2ThetaT >= 1) return false;
	f32 cosThetaT = std::sqrt(1 - sin2ThetaT);
	*wt = eta * -wi + (eta * cosThetaI - cosThetaT) * Vec3(n);
	return true;

}
