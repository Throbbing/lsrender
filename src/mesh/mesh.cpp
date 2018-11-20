#include<mesh/mesh.h>
#include<record/record.h>
#include<scene/scene.h>

bool ls::Mesh::intersect(ls_Param_In const ls::Ray & ray, ls_Param_In const RTCRecord & rtc, ls_Param_Out Record * rec) const
{
	if(mGeomID != rtc.geomID)
		return false;


	Unimplement;
	return false;
}

bool ls::Mesh::occlude(ls_Param_In const ls::Ray & ray, ls_Param_In const RTCRecord & rtc) const
{
	if (mGeomID != rtc.geomID)
		return false;

	Unimplement;
	return false;
}

void ls::Mesh::applyAreaLight(const Spectrum & power)
{
	mLightPower = power;
	if (!mLightPower.isBlack())
		mIsAreaLight = true;
}

bool ls::Mesh::isAreaLight()
{
	return mIsAreaLight;
}
