#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<record/record.h>
#include<math/transform.h>
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
	class Mesh
	{
		friend Scene;

		
	public:
		Mesh(const EMesh_Type& type):mMeshType(type) {}
		virtual ~Mesh() {}

		//attach this meth to scene
		//The mesh cant be detected until attaching to scene 
		void attachScene(std::shared_ptr<Scene> scene);
		
		//detach this mesh from scene
		void detachScene();

		
		virtual bool intersect(ls_Param_In const ls::Ray& ray,
			ls_Param_In const RTCRecord& rtc,
			ls_Param_Out Record* rec) const ;

		virtual bool occlude(ls_Param_In const ls::Ray& ray,
			ls_Param_In const RTCRecord& rtc) const ;
		

		virtual bool sample(ls_Param_In Sampler* sampler,
			ls_Param_Out Record* rec) const = 0;

		virtual bool sample(ls_Param_In Sampler* sampler,
			ls_Param_In const Record* refRec,
			ls_Param_Out Record* rec) const = 0;

		

		
		virtual void applyTransform(const Transform& transform) = 0;
		
		//commit changes to geometry
		virtual void commit() = 0;

		virtual f32 pdf(ls_Param_In const Record* refRec) const = 0;

		virtual void subdivide(ESubdivision_Type type, u32 count) const = 0;

	protected:
		s32							mGeomID = -1;
		const EMesh_Type			mMeshType;
		std::shared_ptr<Scene>		mScene = nullptr;
		
	};
}