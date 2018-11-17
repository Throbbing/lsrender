#pragma once
#include<mesh/mesh.h>
#include<math/transform.h>
#include<spectrum/spectrum.h>
#include<embree3/rtcore.h>

namespace ls
{
	class TriMesh :public Mesh
	{
		friend ResourceManager;
	public:
		TriMesh() :Mesh(EMesh_TriMesh) {}
		TriMesh(const std::vector<Point3>& vertices,
			const std::vector<Normal>& normals,
			const std::vector<Point2>& uvs,
			const std::vector<u32>& indices);


		//get functions
		std::vector<Point3>			getVertices()  const { return mVertices; }
		std::vector<Normal>			getNormals()  const { return mNormals; }
		std::vector<Point2>			getUVs() const { return mUVs; }
		std::vector<u32>			getIndices() const { return mIndices; }

		auto						getVertexSize() const { return mVertices.size(); }
		auto						getNormalSize() const { return mNormals.size(); }
		auto						getUVSize() const { return mUVs.size(); }
		auto						getIndexSize() const { return mIndices.size(); }

		virtual bool intersect(ls_Param_In const ls::Ray& ray,
			ls_Param_In const RTCRecord& rtc,
			ls_Param_Out Record* rec) const override;

		virtual bool occlude(ls_Param_In const ls::Ray& ray,
			ls_Param_In const RTCRecord& rtc) const override;


		virtual void applyTransform(const Transform& transform) override;


		virtual void commit() override;
		

		virtual bool sample(ls_Param_In Sampler* sampler,
			ls_Param_Out Record* rec) const  override ;

		virtual bool sample(ls_Param_In Sampler* sampler,
			ls_Param_In const Record* refRec,
			ls_Param_Out Record* rec) const  override;

		virtual 

		virtual f32 pdf(ls_Param_In const Record* refRec) const override;

		virtual void subdivide(ESubdivision_Type type, u32 count) const override;


	protected:
		bool						mIsValid = false;
		std::vector<Point3>			mVertices;
		std::vector<Normal>			mNormals;
		std::vector<Point2>			mUVs;
		std::vector<u32>			mIndices;
		Transform					mO2W;

		RTCGeometry					mEmbreeGem;

	};
}
