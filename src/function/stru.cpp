#include<function/stru.h>
#include<function/log.h>
void ls::AABB::unionAABB(const AABB & aabb)
{
	minP.x = std::min(minP.x, aabb.minP.x);
	minP.y = std::min(minP.y, aabb.minP.y);
	minP.z = std::min(minP.z, aabb.minP.z);

	maxP.x = std::max(maxP.x, aabb.maxP.x);
	maxP.y = std::max(maxP.y, aabb.maxP.y);
	maxP.z = std::max(maxP.z, aabb.maxP.z);
}

bool ls::AABB::inside(const Point3 & p)
{
	for (s32 i = 0; i < 3; ++i)
	{
		if (p[i]< minP[i] || p[i]>maxP[i])
			return false;
	}
	return true;
}

bool ls::AABB::intersect(ls_Param_In const Ray & ray, ls_Param_Out f32 * t)
{
	Unimplement;
	return false;
}
