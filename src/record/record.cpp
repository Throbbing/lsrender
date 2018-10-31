#include<record/record.h>
#include<light/light.h>
#include<scatter/scatter.h>
namespace ls
{
	Point3 ls::CameraSampleRecord::getPosition()
	{
		Unimplement;
		return Point3();
	}

	Normal ls::CameraSampleRecord::getNormal()
	{
		Unimplement;
		return Normal();
	}




	Point3 LightSampleRecord::getPosition()
	{
		Unimplement;
		return Point3();
	}
	Normal LightSampleRecord::getNormal()
	{
		Unimplement;
		return Normal();
	}



	Point3 MeshSampleRecord::getPosition()
	{
		Unimplement;
		return Point3();
	}
	Normal MeshSampleRecord::getNormal()
	{
		Unimplement;
		return Normal();
	}

	Point3 ScatteringRecord::getPosition()
	{
		Unimplement;
		return Point3();
	}

	Normal ScatteringRecord::getNormal()
	{
		Unimplement;
		return Normal();
	}

	


	Point3 MediumSampleRecord::getPosition()
	{
		Unimplement;
		return Point3();
	}
	Normal MediumSampleRecord::getNormal()
	{
		Unimplement;
		return Normal();
	}


	Point3 SurfaceSampleRecord::getPosition()
	{
		Unimplement;
		return Point3();
	}

	Normal SurfaceSampleRecord::getNormal()
	{
		Unimplement;
		return Normal();
	}

	Point3 IntersectionRecord::getPosition()
	{
		Unimplement;
		return Point3();
	}
	Normal IntersectionRecord::getNormal()
	{
		Unimplement;
		return Normal();
	}

	Light * IntersectionRecord::getAreaLight()
	{
		return areaLight;
	}

	ScatteringFunction * IntersectionRecord::getBSDF()
	{
		return nullptr;
	}






}