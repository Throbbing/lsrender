#include<math/math.h>
#include<math/vector.h>
constexpr f32 ls::lsMath::Infinity;
constexpr f32 ls::lsMath::Epsilon;

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
