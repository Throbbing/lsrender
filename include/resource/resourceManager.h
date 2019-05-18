#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<config/module.h>
#include<map>

namespace ls
{
	class Path;
	class ResourceManager
	{
	public:
		static std::vector<MeshPtr> loadMeshFromFile(const Path& path,
			const std::string& fileName);
		static std::vector<MeshPtr> loadMeshFromFileW(const Path& path,
			const std::wstring& fileName);

		static ls_Smart(ls::Texture) loadTextureFromFile(const Path& path,
			const std::string& fileName);
		static ls_Smart(ls::Texture) loadTextureFromFileW(const Path& path,
			const std::wstring& fileName);

		static ls_Smart(ls::Scene)   createSceneObj();

		static CameraPtr			 createCamera(ParamSet& paramSet);
		static ScatteringFunctionPtr createScatteringFunction(ParamSet& paramSet);
		static FilmPtr				 createFilm(ParamSet& paramSet);
		static LightPtr				 createLight(ParamSet& paramSet);
		static MaterialPtr			 createMaterial(ParamSet& paramSet);
		static SamplerPtr			 createSampler(ParamSet& paramSet);
		static TexturePtr			 createTexture(ParamSet& paramSet);
		static MeshPtr				 createMesh(ParamSet& paramSet);


		static void write2File(ls::Texture* texture,
			const Path& path,
			const std::string& fileName);
		static void write2FileW(ls::Texture* texture,
			const Path& path,
			const std::wstring& fileName);

		

//		static ls_Smart(ls::Material) createMaterial(ParamSet paramSet)

	private:
		struct MeshIndex
		{
			s32 start;
			s32 end;
		};
		static std::map<std::string, MeshIndex>				mMeshIndices;
		static std::vector<ls::MeshPtr>						mMeshs;
		static std::map<std::string, ls_Smart(ls::Texture)>	mTextures;
		static std::vector<ls::ModulePtr>								mModules;
	};
}
