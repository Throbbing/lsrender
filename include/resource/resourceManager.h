#pragma once
#include<config\config.h>
#include<config\declaration.h>

namespace ls
{
	class ResourceManager
	{
	public:
		static ls_Smart(ls::Mesh) loadMeshFromFile(const std::string& path,
			const std::string& fileName);
		static ls_Smart(ls::Texture) loadTextureFromFile(const std::string& path,
			const std::string& fileName);
	};
}
