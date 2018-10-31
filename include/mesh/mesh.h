#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<record/record.h>

namespace ls
{
	class Mesh
	{
		friend Scene;
		enum ESubdivision_Type
		{
			ESubdivision_Loop,
			ESubdivision_Butterfly
		};
		
	public:
		Mesh() {}
		virtual ~Mesh() {}

		void attachScene(std::shared_ptr<Scene> scene);
		void detachScene();

		virtual bool intersect(ls_Param_In const ls::Ray& ray,
			ls_Param_In const RTCRecord& rtc,
			ls_Param_Out Record* rec);

		virtual bool occlude(ls_Param_In const ls::Ray& ray,
			ls_Param_In const RTCRecord& rtc);
		

		virtual bool sample(ls_Param_In Sampler* sampler,
			ls_Param_Out Record* rec) = 0;

		virtual bool sample(ls_Param_In Sampler* sampler,
			ls_Param_In const Record* refRec,
			ls_Param_Out Record* rec) = 0;

		virtual f32 pdf(ls_Param_In const Record* refRec) = 0;

		virtual void subdivide(ESubdivision_Type type, u32 count) = 0;

	protected:
		s32							mGeomID = -1;
		std::shared_ptr<Scene>		mScene = nullptr;
	};
}