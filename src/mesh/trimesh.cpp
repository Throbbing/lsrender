#include <mesh/trimesh.h>
#include<config/common.h>
#include<math/transform.h>
#include<function/func.h>
ls::TriMesh::TriMesh(const std::vector<Point3>& vertices, 
	const std::vector<Normal>& normals, 
	const std::vector<Point2>& uvs,
	const std::vector<u32>& indices):Mesh(EMesh_TriMesh)
{
	if (!vertices.empty() && !indices.empty())
		mIsValid = true;

	if (!mIsValid)
		return;

	mVertices = vertices;
	mNormals = normals;
	mUVs = uvs;
	mIndices = indices;

	mEmbreeGem = nullptr;
}

bool ls::TriMesh::intersect(ls_Param_In const ls::Ray & ray, 
	ls_Param_In const RTCRecord & rtc, 
	ls_Param_Out Record * rec) const
{
	if (rtc.geomID != mGeomID)
		return false;


	//Since intersect test has been completed by embree
	//we only need to compute differential components
	auto dgRec = IntersectionPtrCast(rec);

	

	


	return true;
}

bool ls::TriMesh::occlude(ls_Param_In const ls::Ray & ray, 
	ls_Param_In const RTCRecord & rtc) const
{
	return false;
}

void ls::TriMesh::applyTransform(const Transform & transform)
{
	mO2W = transform;
}

void ls::TriMesh::commit()
{
	if (mEmbreeGem)
	{
		rtcReleaseGeometry(mEmbreeGem);
		mEmbreeGem = nullptr;
	}
	if (!mIsValid)
		ls_AssertMsg(false, "Invalid Geometry Commit");

	mEmbreeGem = rtcNewGeometry(lsEmbree::hw.rtcDevice, RTC_GEOMETRY_TYPE_TRIANGLE);
	ls_AssertMsg(mEmbreeGem, "New Geometry Failed ");

	{
		auto vertices = (Point3*)rtcSetNewGeometryBuffer(mEmbreeGem, RTC_BUFFER_TYPE_VERTEX,
			0, RTC_FORMAT_FLOAT3, sizeof(Point3), mVertices.size());
		ls_AssertMsg(vertices, "Invalid Set Vertex Geometry Buffer");

		for (int i = 0; i < mVertices.size(); ++i)
		{
			vertices[i] = mO2W(mVertices[i]);
		}
	}
	
	{
		auto indices = rtcSetNewGeometryBuffer(mEmbreeGem, RTC_BUFFER_TYPE_INDEX, 0,
			RTC_FORMAT_UINT, sizeof(u32), mIndices.size());
		ls_AssertMsg(indices, "Invalid Set Index Geometry Buffer");

		std::memcpy(indices, &mIndices[0],
			sizeof(mIndices[0]) * mIndices.size());
	}

	u32 vertexAttriSlot = 0;
	if (!mNormals.empty())
	{
		auto normals = (Normal*)rtcSetNewGeometryBuffer(mEmbreeGem,
			RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
			vertexAttriSlot,
			RTC_FORMAT_FLOAT3, sizeof(Normal), mNormals.size());
		ls_Assert(normals, "Invalid Set Normal Geometry Buffer");
		
		for (int i = 0; i < mNormals.size(); ++i)
		{
			normals[i] = normalize(mO2W(mNormals[i]));
		}
		++vertexAttriSlot;
	}

	if (!mUVs.empty())
	{
		auto uvs = (Point2*)rtcSetNewGeometryBuffer(mEmbreeGem,
			RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
			vertexAttriSlot,
			RTC_FORMAT_FLOAT2, sizeof(Point2), mUVs.size());
		ls_Assert(uvs, "Invalid Set UV Geometry Buffer");

		for (int i = 0; i < mUVs.size(); ++i)
		{
			uvs[i] = mUVs[i];
		}
	}

	rtcCommitGeometry(mEmbreeGem);
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


