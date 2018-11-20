#include<resource\resourceManager.h>
#include<3rd\tiny_obj_loader.h>
#include<function\log.h>
#include<mesh\mesh.h>
#include<mesh\trimesh.h>

#include<cstring>


ls_Smart(ls::Mesh) ls::ResourceManager::loadMeshFromFile(const std::string & path, const std::string & fileName)
{
	if (mMeshs.find(fileName) != mMeshs.end())
		return mMeshs[fileName];

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	std::string warn;
	
	if (!tinyobj::LoadObj(&attrib, &shapes,
		&materials, &err,&warn, (path + fileName).c_str()))
	{
		std::cout << err << std::endl;
	}

	std::vector<Point3> positions;
	std::vector<Normal> normals;
	std::vector<Point2>	texs;
	std::vector<u32>	indices;
	for (size_t s = 0; s < shapes.size(); ++s)
	{
		size_t indexOffset = 0;
		for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); ++f)
		{
			auto fv = shapes[s].mesh.num_face_vertices[f];

			Point3 p;
			Normal n;
			Point2 t;
			for (size_t v = 0; v < fv; ++v)
			{
				auto idx = shapes[s].mesh.indices[indexOffset + v];
				auto vx = attrib.vertices[3 * idx.vertex_index + 0];
				auto vy = attrib.vertices[3 * idx.vertex_index + 1];
				auto vz = attrib.vertices[3 * idx.vertex_index + 2];

				p = Point3(vx, vy, vz);

				if (!attrib.normals.empty())
				{
					auto nx = attrib.normals[3 * idx.normal_index + 0];
					auto ny = attrib.normals[3 * idx.normal_index + 1];
					auto nz = attrib.normals[3 * idx.normal_index + 2];
					n = Normal(nx, ny, nz);
				}
				if (!attrib.texcoords.empty())
				{
					auto tx = attrib.texcoords[2 * idx.texcoord_index + 0];
					auto ty = attrib.texcoords[2 * idx.texcoord_index + 1];
					t = Point2(tx, ty);
				}
				indices.push_back(positions.size());
				positions.push_back(p);
				if (!attrib.normals.empty()) normals.push_back(n);
				if (!attrib.texcoords.empty()) texs.push_back(t);
			}
			indexOffset += fv;
		}
		
	}

	auto mesh = ls_MakeSmart(TriMesh)(positions, normals, texs, indices);
	mMeshs[fileName] = mesh;
	return mesh;
}

ls_Smart(ls::Texture) ls::ResourceManager::loadTextureFromFile(const std::string & path, const std::string & fileName)
{
	return ls_Smart(ls::Texture)();
}

ls_Smart(ls::Scene) ls::ResourceManager::createSceneObj()
{
	return ls_Smart(ls::Scene)();
}
