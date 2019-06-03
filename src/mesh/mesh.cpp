#include<light/areaLight.h>
#include<mesh/mesh.h>
#include<record/record.h>
#include<resource/resourceManager.h>
#include<resource/xmlHelper.h>
#include<scene/scene.h>


bool ls::Mesh::intersect(ls_Param_In const ls::Ray & ray, ls_Param_In const RTCRecord & rtc, ls_Param_Out IntersectionRecord * rec) const
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

void ls::Mesh::applyAreaLight(const Spectrum & radiance)
{
	ParamSet paramSet = ParamSet("light", "areaLight", "", "");
	paramSet.addSpectrum("radiance", radiance);

	mAreaLight = ResourceManager::createLight(paramSet);

	dynamic_cast<AreaLight*>(mAreaLight)->attachMesh(MeshPtr(this));
}

bool ls::Mesh::isAreaLight()
{
	return mAreaLight;
}
