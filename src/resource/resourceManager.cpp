#include <resource\resourceManager.h>
#include<mesh\mesh.h>
#include<mesh\trimesh.h>

ls_Smart(ls::Mesh) ls::ResourceManager::loadMeshFromFile(const std::string & path, const std::string & fileName)
{
	return ls_Smart(Mesh)();
}

ls_Smart(ls::Texture) ls::ResourceManager::loadTextureFromFile(const std::string & path, const std::string & fileName)
{
	return ls_Smart(ls::Texture)();
}
