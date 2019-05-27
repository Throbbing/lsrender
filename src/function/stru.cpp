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

void ls::AABB::unionPoint(const Point3 & p)
{
	if (p.x < minP.x) minP.x = p.x;
	if (p.y < minP.y) minP.y = p.y;
	if (p.z < minP.z) minP.z = p.z;

	if (p.x > maxP.x) maxP.x = p.x;
	if (p.y > maxP.y) maxP.y = p.y;
	if (p.z > maxP.z) maxP.z = p.z;
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

f32 ls::AABB::maxExtent()
{
	auto x = maxP.x - minP.x;
	auto y = maxP.y - minP.y;
	auto z = maxP.z - minP.z;

	return std::min(x, std::min(y, z));
}

bool ls::AABB::intersect(ls_Param_In const Ray & ray, ls_Param_Out f32 * t)
{
	Unimplement;
	return false;
}
