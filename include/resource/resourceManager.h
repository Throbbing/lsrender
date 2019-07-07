#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
#include<map>
#include<string>
namespace ls
{
	class Path;

	struct ImageData;
	class ResourceManager
	{
	public:

		static void clear();

		static void setPath(const std::string& path) { mPath = path; }

		static std::vector<MeshPtr> loadMeshFromFile(Path fullPath);

		static ImageData loadTextureFromFile(Path fullPath);


		static ls::ScenePtr   createSceneObj();

		static RenderAlgorithmPtr	 createAlgorithm(ParamSet& paramSet);
		static CameraPtr			 createCamera(ParamSet& paramSet);
		static ScatteringFunctionPtr createScatteringFunction(ParamSet& paramSet);
		static FilmPtr				 createFilm(ParamSet& paramSet);
		static LightPtr				 createLight(ParamSet& paramSet);
		static MaterialPtr			 createMaterial(ParamSet& paramSet);
		static SamplerPtr			 createSampler(ParamSet& paramSet);
		static TexturePtr			 createTexture(ParamSet& paramSet);
		static std::vector<MeshPtr>	 createMesh(ParamSet& paramSet);
		static MeshPtr				 createRectangleMesh(ParamSet& paramSet);

		static void write2File(ls::Texture* texture,
			const Path& fullPath);


		

//		static ls_Smart(ls::Material) createMaterial(ParamSet paramSet)

	private:
		struct MeshIndex
		{
			s32 start;
			s32 end;
		};
		static std::map<std::string, MeshIndex>				mMeshIndices;
		static std::vector<ls::MeshPtr>						mMeshs;
		static std::map<std::string, ls::ImageData>			mImageDatas;
		static std::vector<ls::ModulePtr>					mModules;

		static std::string									mPath;
	};
}
