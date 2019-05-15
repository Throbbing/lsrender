#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/lsPtr.h>
#include<map>

namespace ls
{
	class Path;
	class ResourceManager
	{
	public:
		static ls_Smart(ls::Mesh) loadMeshFromFile(const Path& path,
			const std::string& fileName);
		static ls_Smart(ls::Mesh) loadMeshFromFileW(const Path& path,
			const std::wstring& fileName);

		static ls_Smart(ls::Texture) loadTextureFromFile(const Path& path,
			const std::string& fileName);
		static ls_Smart(ls::Texture) loadTextureFromFileW(const Path& path,
			const std::wstring& fileName);

		static ls_Smart(ls::Scene)   createSceneObj();

		static void write2File(ls::Texture* texture,
			const Path& path,
			const std::string& fileName);
		static void write2FileW(ls::Texture* texture,
			const Path& path,
			const std::wstring& fileName);

		

//		static ls_Smart(ls::Material) createMaterial(ParamSet paramSet)

	private:
		static std::map<std::string, ls_Smart(ls::Mesh)>				mMeshs;
		static std::map<std::string, ls_Smart(ls::Texture)>				mTextures;
	};
}
