#include<math/math.h>
#include<math/vector.h>
constexpr f32 ls::lsMath::Infinity;
constexpr f32 ls::lsMath::Epsilon;

f32 ls::lsMath::distance(Point3 p1, Point3 p2)
{
	return Vec3(p1-p2).length();
}
