#pragma once
#include<config\config.h>
#include<config\declaration.h>
#include<map>
namespace ls
{
	class ResourceManager
	{
	public:
		static ls_Smart(ls::Mesh) loadMeshFromFile(const std::string& path,
			const std::string& fileName);
		static ls_Smart(ls::Texture) loadTextureFromFile(const std::string& path,
			const std::string& fileName);
		static ls_Smart(ls::Scene)   createSceneObj();

	private:
		static std::map<std::string, ls_Smart(ls::Mesh)>				mMeshs;
		static std::map<std::string, ls_Smart(ls::Texture)>				mTextures;
	};
}
