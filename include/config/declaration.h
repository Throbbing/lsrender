#pragma once


namespace ls
{
	//Distribution
	class Distribution1D;
	class Distribution2D;
	
	//Record
	struct RTCRecord;					
	struct CameraSpwanRayRecord;
	struct CameraSampleRecord;
	struct LightSampleRecord;
	struct ScatteringRecord;
	struct MeshSampleRecord;
	struct SurfaceSampleRecord;
	struct MediumSampleRecord;
	struct IntersectionRecord;




	//Module
	class Module;

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
	class XMLParser;
	class ParamSet;

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
	struct SceneRenderBlock;


	//About Spectrum
	class Spectrum;


	//About Thread
//	class ThreadWaker;
	class Thread;
	class ThreadTask;
//	class ThreadEvent;
//	class ThreadPool;
	class QueuedThreadPool;

	//About File
	class Path;

	//About Timer
	class Timer;

	//About Realtime
	class RealtimeRenderer;
	class RealtimeRenderCommand;
	class Canvas;

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