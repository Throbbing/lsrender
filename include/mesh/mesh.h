#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
#include<record/record.h>
#include<math/transform.h>
#include<spectrum/spectrum.h>
#include<function/stru.h>
namespace ls
{
	enum ESubdivision_Type
	{
		ESubdivision_Loop,
		ESubdivision_Butterfly
	};

	enum EMesh_Type
	{
		EMesh_Geometry = 0,
		EMesh_TriMesh,
		EMesh_Bezier,
		EMesh_TypeCount
	};
	class Mesh:public Module
	{
		friend Scene;
		
		
	public:
		Mesh(const EMesh_Type& type,
			const std::string& id = "mesh"):Module(id),mMeshType(type) {}
		virtual ~Mesh() {}


		AABB getAABB() const { return mAABB; }
		
		virtual bool intersect(ls_Param_In const ls::Ray& ray,
			ls_Param_In const RTCRecord& rtc,
			ls_Param_Out IntersectionRecord* rec) const ;


		virtual bool occlude(ls_Param_In const ls::Ray& ray,
			ls_Param_In const RTCRecord& rtc) const ;
		

		virtual bool sample(ls_Param_In Sampler* sampler,
			ls_Param_Out MeshSampleRecord* rec) const = 0;

		virtual bool sample(ls_Param_In Sampler* sampler,
			ls_Param_In const MeshSampleRecord* refRec,
			ls_Param_Out MeshSampleRecord* rec) const = 0;

		

		
		virtual void applyTransform(const Transform& transform) = 0;
		virtual void applyAreaLight(const Spectrum& radiance);
		virtual void applyMaterial(MaterialPtr material) { mMaterial = material; }
		virtual bool isAreaLight();
		
		virtual LightPtr getAreaLight() const { return mAreaLight; }
		virtual f32  getArea() const = 0;
		//commit changes to geometry
		virtual void commit() = 0;

		virtual f32 pdf(ls_Param_In ls_Param_Out MeshSampleRecord* refRec) const = 0;

		virtual void subdivide(ESubdivision_Type type, u32 count) const = 0;

		
	protected:
		s32							mGeomID = -1;
		const EMesh_Type			mMeshType;
		std::shared_ptr<Scene>		mScene = nullptr;
		LightPtr					mAreaLight = nullptr;
		MaterialPtr					mMaterial;
		AABB						mAABB;

		RTCGeometry					mEmbreeGem = nullptr;
	};
}