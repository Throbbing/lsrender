#pragma once


namespace ls
{
	//Record
	class RTCRecord;
	class Record;
	class CameraSampleRecord;
	class LightSampleRecord;
	class MeshSampleRecord;
	class ScatteringRecord;
	class SurfaceSampleRecord;
	class MediumSampleRecord;
	class IntersectionRecord;

	//About object
	class Light;
	class Mesh;
	class Material;


	//About light transport
	class ScatteringFunction;
	class Medium;
	class Phase;
	class Texture;

	//About Geometry
	class Ray;
	class DifferentialRay;
	class AABB;


	//About Algorithm
	class RenderAlgorithm;


	//About resource manage
	class MemoryAllocater;
	class ResourceManager;
	class XMLPackage;

	//About Random
	class RNG;

	//About Sample
	class Sampler;
	class CameraSample;


	//About Camera and film
	class Camera;
	class Film;

	
	
	//About Scene
	class Scene;


	//About Spectrum
	class Spectrum;

	//About Math
	struct Vec2;
	struct Vec3;
	struct Vec4;
	struct Point2;
	struct Point3;
	struct Normal;
	class Mat4x4;
	class Transform;

	
}