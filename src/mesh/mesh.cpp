#include<mesh/mesh.h>
#include<record/record.h>
#include<scene/scene.h>
void ls::Mesh::attachScene(std::shared_ptr<Scene> scene)
{
	mScene = scene;
	mGeomID = scene->addMesh(this);

}

void ls::Mesh::detachScene()
{
	mScene->deleteMesh(this);
	mScene = nullptr;
	mGeomID = -1;
	
}

bool ls::Mesh::intersect(ls_Param_In const ls::Ray & ray, ls_Param_In const RTCRecord & rtc, ls_Param_Out Record * rec)
{
	if(mGeomID != rtc.geomID)
		return false;


	Unimplement;
	return false;
}

bool ls::Mesh::occlude(ls_Param_In const ls::Ray & ray, ls_Param_In const RTCRecord & rtc)
{
	if (mGeomID != rtc.geomID)
		return false;

	Unimplement;
	return false;
}
