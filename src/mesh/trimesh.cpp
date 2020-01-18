#include <mesh/trimesh.h>
#include<config/common.h>
#include<math/transform.h>
#include<math/frame.h>
#include<function/func.h>
#include<function/stru.h>
#include<record/record.h>
#include<sampler/sampler.h>
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
	ls_Param_Out IntersectionRecord * rec) const
{
	if (rtc.geomID != mGeomID)
		return false;


	//Since intersect test has been completed by embree
	//we only need to compute differential components
	Point position;
	Normal ng = normalize(Normal(rtc.rayHit.hit.Ng_x,
		rtc.rayHit.hit.Ng_y,
		rtc.rayHit.hit.Ng_z));
	Normal ns = ng;
	Vec2 uv = Vec2(rtc.rayHit.hit.u,
		rtc.rayHit.hit.v);
	Vec3 dpdu, dpdv;

	int slot = 0;
	if (!mNormals.empty())
	{
		rtcInterpolate0(mEmbreeGem, rtc.primID,
			rtc.rayHit.hit.u,
			rtc.rayHit.hit.v,
			RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
			slot++, &ns.x, 3);

		if (!validNormal(ns))
			ns = ng;
	}


	if (!mUVs.empty())
	{
		rtcInterpolate0(mEmbreeGem, rtc.primID,
			rtc.rayHit.hit.u,
			rtc.rayHit.hit.v,
			RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
			slot, &uv.x, 2);
	}

	rtcInterpolate0(mEmbreeGem, rtc.primID,
		rtc.rayHit.hit.u,
		rtc.rayHit.hit.v,
		RTC_BUFFER_TYPE_VERTEX, 0, &position.x, 3);
	
	Point2 uv0, uv1, uv2;
	if (!mUVs.empty())
	{
		u32 v0 = rtc.primID * 3 + 0;
		u32 v1 = rtc.primID * 3 + 1;
		u32 v2 = rtc.primID * 3 + 2;
		uv0 = mUVs[mIndices[v0]];
		uv1 = mUVs[mIndices[v1]];
		uv2 = mUVs[mIndices[v2]];
		f32 u02 = uv0.x - uv2.x;
		f32 u12 = uv1.x - uv2.x;
		f32 u01 = uv0.x - uv1.x;
		f32 v02 = uv0.y - uv2.y;
		f32 v01 = uv0.y - uv1.y;
		f32 v12 = uv1.y - uv2.y;

		

		auto p02 = mVertices[v0] - mVertices[v2];
		auto p01 = mVertices[v0] - mVertices[v1];
		auto p12 = mVertices[v1] - mVertices[v2];


		auto det = u02*v12 - v02*u02;
		if (!lsMath::closeZero(det))
		{
			auto invDet = 1.f / det;
			dpdu = Vec3(( v12 * p02 - v02 * p12)) * invDet;
			dpdv = Vec3((-u12 * p02 + v02 * p12)) * invDet;
		}
		else
		{
			rtcInterpolate1(mEmbreeGem,
				rtc.primID,
				rtc.rayHit.hit.u,
				rtc.rayHit.hit.v,
				RTC_BUFFER_TYPE_VERTEX,
				0, nullptr,
				&dpdu.x, &dpdv.x,
				3);
		}
	}
	else
	{
		rtcInterpolate1(mEmbreeGem,
			rtc.primID,
			rtc.rayHit.hit.u,
			rtc.rayHit.hit.v,
			RTC_BUFFER_TYPE_VERTEX,
			0, nullptr,
			&dpdu.x, &dpdv.x,
			3);
	}


	auto dgRec = rec;

	dgRec->position = position;
	dgRec->ng = ng;
	dgRec->ns = ns;
	dgRec->uv = uv;
	dgRec->dpdu = mO2W(dpdu);
	dgRec->dpdv = mO2W(dpdv);
	dgRec->dpdx = Vec3(0.f);
	dgRec->dpdy = Vec3(0.f);
	dgRec->dndu = Vec3(0.f);
	dgRec->dndv = Vec3(0.f);
	dgRec->dndx = Vec3(0.f);
	dgRec->dndy = Vec3(0.f);
	dgRec->material = mMaterial;
	dgRec->areaLight = mAreaLight;
	return true;
}

bool ls::TriMesh::occlude(ls_Param_In const ls::Ray & ray, 
	ls_Param_In const RTCRecord & rtc) const
{
	if (mGeomID != rtc.geomID)
		return false;

	return true;
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
		mAABB = AABB();
		auto vertices = (Point3*)rtcSetNewGeometryBuffer(mEmbreeGem, RTC_BUFFER_TYPE_VERTEX,
			0, RTC_FORMAT_FLOAT3, sizeof(Point3), mVertices.size());
		ls_AssertMsg(vertices, "Invalid Set Vertex Geometry Buffer");

		for (int i = 0; i < mVertices.size(); ++i)
		{
			auto v = mO2W(mVertices[i]);
			vertices[i] = v;
			mAABB.unionPoint(v);
		}
		mArea = 0;
		std::vector<f32> areas;

		if (mAreasDistribution) delete mAreasDistribution;

		for (int i = 0; i < mVertices.size() / 3; ++i)
		{
			auto v0 = mO2W(mVertices[i * 3 + 0]);
			auto v1 = mO2W(mVertices[i * 3 + 1]);
			auto v2 = mO2W(mVertices[i * 3 + 2]);

			auto v01 = Vec3(v0 - v1); auto a = v01.length();
			auto v02 = Vec3(v0 - v2); auto b = v02.length();

			v01 /= a;
			v02 /= b;
			auto cos = dot(v01, v02);
			auto sin = std::sqrtf(1.f - cos *cos);

			auto area = .5f * a *b * sin;

			mArea += area;
			areas.push_back(area);
		}
		mAreasDistribution = new Distribution1D(&areas[0], areas.size());

	}
	
	{
		auto indices = (u32*)rtcSetNewGeometryBuffer(mEmbreeGem, RTC_BUFFER_TYPE_INDEX, 0,
			RTC_FORMAT_UINT3, sizeof(u32) * 3, mIndices.size() / 3);
		auto tt = rtcGetDeviceError(lsEmbree::hw.rtcDevice);
		ls_AssertMsg(indices, "Invalid Set Index Geometry Buffer");

		

		std::memcpy(indices, &mIndices[0],
			sizeof(mIndices[0]) * mIndices.size());
	}

	{
		int vaCount = 0;
		if (!mNormals.empty()) vaCount++;
		if (!mUVs.empty()) vaCount++;
		if (vaCount != 0)
			rtcSetGeometryVertexAttributeCount(mEmbreeGem, vaCount);
	}

	u32 vertexAttriSlot = 0;
	if (!mNormals.empty())
	{
		Normal* normals = nullptr;
		normals = (Normal*)rtcSetNewGeometryBuffer(mEmbreeGem,
			RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
			vertexAttriSlot,
			RTC_FORMAT_FLOAT3, sizeof(Normal), mNormals.size());
		auto tt = rtcGetDeviceError(lsEmbree::hw.rtcDevice);
		ls_AssertMsg(normals, "Invalid Set Normal Geometry Buffer");
		
		for (int i = 0; i < mNormals.size(); ++i)
		{
			auto n = normalize(mO2W(mNormals[i]));
			if (isnan(n.x) || isnan(n.y) || isnan(n.z))
			{
				n = Normal(1, 0, 0);
			}
			normals[i] = n;
		}
		++vertexAttriSlot;
	}

	if (!mUVs.empty())
	{
		auto uvs = (Point2*)rtcSetNewGeometryBuffer(mEmbreeGem,
			RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
			vertexAttriSlot,
			RTC_FORMAT_FLOAT2, sizeof(Point2), mUVs.size());
		auto tt = rtcGetDeviceError(lsEmbree::hw.rtcDevice);
		ls_AssertMsg(uvs, "Invalid Set UV Geometry Buffer");

		for (int i = 0; i < mUVs.size(); ++i)
		{
			uvs[i] = mUVs[i];
		}
	}
	auto ttt = (Point3*)rtcGetGeometryBufferData(mEmbreeGem, RTC_BUFFER_TYPE_VERTEX, 0);
	auto tttt = (u32*)rtcGetGeometryBufferData(mEmbreeGem, RTC_BUFFER_TYPE_INDEX, 0);

	rtcCommitGeometry(mEmbreeGem);

}

std::string ls::TriMesh::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Mesh: " << "TriMesh" << std::endl;
	oss << ls_Separator << std::endl;

	return oss.str();
}

bool ls::TriMesh::sample(ls_Param_In Sampler * sampler,
	ls_Param_Out MeshSampleRecord * rec) const
{
	//选择三角形
	int selectedTriIndex = mAreasDistribution->SampleDiscrete(sampler->next1D());

	auto p0 = mVertices[mIndices[selectedTriIndex * 3 + 0]];
	auto p1 = mVertices[mIndices[selectedTriIndex * 3 + 1]];
	auto p2 = mVertices[mIndices[selectedTriIndex * 3 + 2]];

	//三角形采样
	Point2 b;
	MonteCarlo::sampleTriangle(sampler->next2D(), &b);
	//	MonteCarlo::sampleTriangle(Point2(0.5,1.f), &b);
	auto p = p0 + b.x * (p1 - p0) + b.y * (p2 - p0);

	Normal n;
	if (!mNormals.empty())
	{
		auto n0 = mNormals[mIndices[selectedTriIndex * 3 + 0]];
		auto n1 = mNormals[mIndices[selectedTriIndex * 3 + 1]];
		auto n2 = mNormals[mIndices[selectedTriIndex * 3 + 2]];

		n = b.x * n0 + b.y * n1 + (1.f - b.x - b.y) * n2;
	}
	else
	{

		n = Normal(cross(normalize(Vec3(p1 - p0)), normalize(Vec3(p2 - p0))));
		n = normalize(n);
	}

	rec->samplePosition = mO2W(p);
	rec->pdfA = 1.f / mArea;
	MonteCarlo::sampleCosHemisphere(sampler->next2D(), &rec->sampleDirection, &rec->pdfD);
	rec->surfaceNormal = mO2W(n);
	Frame localFrame(rec->surfaceNormal);

	rec->sampleDirection = localFrame.toWorld(rec->sampleDirection);
	return true;
}

bool ls::TriMesh::sample(ls_Param_In Sampler * sampler, 
	ls_Param_In const MeshSampleRecord * refRec, 
	ls_Param_Out MeshSampleRecord * rec) const
{
	//选择三角形
	int selectedTriIndex = mAreasDistribution->SampleDiscrete(sampler->next1D());
	
	auto p0 = mVertices[mIndices[selectedTriIndex * 3 + 0]];
	auto p1 = mVertices[mIndices[selectedTriIndex * 3 + 1]];
	auto p2 = mVertices[mIndices[selectedTriIndex * 3 + 2]];

	//三角形采样
	Point2 b;
	MonteCarlo::sampleTriangle(sampler->next2D(), &b);
//	MonteCarlo::sampleTriangle(Point2(0.5,1.f), &b);
	auto p =  p0 + b.x * (p1 - p0) + b.y * (p2 - p0);

	Normal n;
	if (!mNormals.empty())
	{
		auto n0 = mNormals[mIndices[selectedTriIndex * 3 + 0]];
		auto n1 = mNormals[mIndices[selectedTriIndex * 3 + 1]];
		auto n2 = mNormals[mIndices[selectedTriIndex * 3 + 2]];

		n = b.x * n0 + b.y * n1 + (1.f - b.x - b.y) * n2;
	}
	else
	{
		
		n = Normal(cross(normalize(Vec3(p1 - p0)), normalize(Vec3(p2 - p0))));
		n = normalize(n);
	}

	rec->samplePosition = mO2W(p);
	rec->surfaceNormal = normalize(mO2W(n));
	rec->pdfA = 1.f / mArea;

	return true;



}

f32 ls::TriMesh::pdf(ls_Param_In ls_Param_Out MeshSampleRecord * refRec) const
{
	refRec->pdfA = 1.f / mArea;
	Frame localFrame(refRec->surfaceNormal);

	refRec->pdfD = MonteCarlo::sampleCosHemispherePdf(localFrame.toLocal(refRec->sampleDirection));

	return refRec->pdfA * refRec->pdfD;
}

void ls::TriMesh::subdivide(ESubdivision_Type type, u32 count) const
{
}

ls::TriMesh::TriMesh(ParamSet & paramSet):Mesh(EMesh_TriMesh)
{
}


