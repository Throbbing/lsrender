#include <mesh/trimesh.h>

ls::TriMesh::TriMesh(const std::vector<Point3>& vertices, 
	const std::vector<Normal>& normals, 
	const std::vector<Point2>& uvs,
	const std::vector<u32>& indices):Mesh(EMesh_TriMesh)
{
	if (!vertices.empty() && !indices.empty())
		mIsValid = true;
}

bool ls::TriMesh::intersect(ls_Param_In const ls::Ray & ray, 
	ls_Param_In const RTCRecord & rtc, 
	ls_Param_Out Record * rec) const
{
	return false;
}

bool ls::TriMesh::occlude(ls_Param_In const ls::Ray & ray, 
	ls_Param_In const RTCRecord & rtc) const
{
	return false;
}

bool ls::TriMesh::sample(ls_Param_In Sampler * sampler,
	ls_Param_Out Record * rec) const
{
	return false;
}

bool ls::TriMesh::sample(ls_Param_In Sampler * sampler, 
	ls_Param_In const Record * refRec, 
	ls_Param_Out Record * rec) const
{
	return false;
}

f32 ls::TriMesh::pdf(ls_Param_In const Record * refRec) const
{
	return f32();
}

void ls::TriMesh::subdivide(ESubdivision_Type type, u32 count) const
{
}


