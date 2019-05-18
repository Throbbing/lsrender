#include<resource/resourceManager.h>
#include<3rd/tiny_obj_loader.h>
#include<function/file.h>
#include<3rd/FreeImage.h>
#include<function/log.h>

#include<camera/pinhole.h>

#include<film/hdr.h>
#include<light/pointLight.h>

#include<material/matte.h>
#include<mesh/mesh.h>
#include<mesh/trimesh.h>
#include<sampler/randomsampler.h>
#include<scatter/lambertian.h>
#include<scene/scene.h>

#include<resource/xmlHelper.h>
#include<texture/constantTexture.h>
#include<texture/imageTexture.h>



#include<cstring>
#include<windows.h>


std::map<std::string, ls::ResourceManager::MeshIndex> ls::ResourceManager::mMeshIndices;
std::vector<ls::MeshPtr>	ls::ResourceManager::mMeshs;
std::map<std::string, ls_Smart(ls::Texture)> ls::ResourceManager::mTextures;



std::vector<ls::MeshPtr> ls::ResourceManager::loadMeshFromFile(const  Path&  path, const std::string & fileName)
{
	if (mMeshIndices.find(fileName) != mMeshIndices.end())
	{
		auto index = mMeshIndices[fileName];
		std::vector<ls::MeshPtr> meshs;
		for (s32 i = index.start; i < index.end; ++i)
		{
			meshs.push_back(mMeshs[i]);
			return meshs;
		}
	}

	tinyobj::attrib_t attrib;
	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string err;
	std::string warn;
	
	
	if (!tinyobj::LoadObj(&attrib, &shapes,
		&materials, &err,&warn, (path.str() +"\\" + fileName).c_str(),nullptr,true))
	{
		std::cout << err << std::endl;
	}

	MeshIndex meshIndex;
	meshIndex.start = mMeshs.size();

	for (size_t s = 0; s < shapes.size(); ++s)
	{
		std::vector<Point3> positions;
		std::vector<Normal> normals;
		std::vector<Point2>	texs;
		std::vector<u32>	indices;
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
		auto mesh = new TriMesh(positions, normals, texs, indices);
		mMeshs.push_back(mesh);
	}
	meshIndex.end = mMeshs.size();

	mMeshIndices[fileName] = meshIndex;

	std::vector<MeshPtr> meshs;
	for (s32 i = meshIndex.start; i < meshIndex.end; ++i)
	{
		meshs.push_back(mMeshs[i]);
	}
	
	return meshs;
}

std::vector<ls::MeshPtr> ls::ResourceManager::loadMeshFromFileW(const Path & path, const std::wstring & fileName)
{
	return loadMeshFromFile(path, ls::toString(fileName));
}

ls_Smart(ls::Texture) ls::ResourceManager::loadTextureFromFile(const  Path& path, const std::string & fileName)
{
	return nullptr;
}

ls_Smart(ls::Texture) ls::ResourceManager::loadTextureFromFileW(const Path & path, const std::wstring & fileName)
{
	std::string fileNameStr = ls::toString(fileName);
	if (mTextures.find(fileNameStr) != mTextures.end())
		return mTextures[fileNameStr];

	
	auto fullPath = ls::Path(path.wstr() + L"\\" + fileName);
	auto ext = fullPath.extension();

	FreeImage_Initialise(true);
	

	FIBITMAP* bmpConverted = nullptr;
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;
	//获取文件类型
	fif = FreeImage_GetFileTypeU(fullPath.wstr().c_str());
	if (fif == FIF_UNKNOWN)
		fif = FreeImage_GetFIFFromFilenameU(fullPath.wstr().c_str());

	ls_AssertMsg(fif != FIF_UNKNOWN, "Unknown Texture Format");
	ls_AssertMsg(FreeImage_FIFSupportsReading(fif), "Unsupport Texture Format");

	//创建纹理 句柄
	FIBITMAP* dib = FreeImage_LoadU(fif, fullPath.wstr().c_str());
	ls_AssertMsg(dib, "Fail to Load Texture from file");


	//所以需要倒置纹理
	FreeImage_FlipVertical(dib);

	//创建纹理数据结构

	//获取并填充纹理信息
	auto width = FreeImage_GetWidth(dib);
	auto height = FreeImage_GetHeight(dib);
	auto bpp = FreeImage_GetBPP(dib);
	auto colorType = FreeImage_GetColorType(dib);
	
	std::vector<Spectrum> data(width *height);

	if (fif != FIF_EXR)
	{
		RGBQUAD rgb;
		for (u32 y = 0; y < height; ++y)
		{
			for (u32 x = 0; x < width; ++x)
			{
				ls_AssertMsg(FreeImage_GetPixelColor(dib, x, y, &rgb), "Invalid FreeImage_GetPixelColor !");

				//rgb中，每个分量为BYTE类型(0~255)
				//将其转换为[0,1]的f32型，并保存在Spectrum中
				float r, g, b;
				r = float(rgb.rgbRed) / 255.f;
				g = float(rgb.rgbGreen) / 255.f;
				b = float(rgb.rgbBlue) / 255.f;
				data[y*width + x] = Spectrum(r, g, b);
			}
		}
	}
	else
	{
		float maxV = 0.f;
		FIBITMAP* rgbfBitmap = FreeImage_ConvertToRGBF(dib);
		ls_AssertMsg(rgbfBitmap, "Invalid FreeImage_ConvertToRGBF ");

		FIRGBF* image = (FIRGBF*)FreeImage_GetBits(rgbfBitmap);
		for (u32 y = 0; y < height; ++y)
		{
			for (u32 x = 0; x < width; ++x)
			{
	
				FIRGBF rgb = image[y*width + x];
				if (rgb.red > maxV) maxV = rgb.red;
				if (rgb.green > maxV) maxV = rgb.green;
				if (rgb.blue > maxV) maxV = rgb.blue;
				data[y*width + x] = Spectrum(rgb.red, rgb.green, rgb.blue);
			}
		}

		std::cout << maxV << std::endl;
	}

	//释放句柄
	FreeImage_Unload(dib);

	ls_Smart(ImageTexture) texture(new ImageTexture(width, height, ls::ERGB, 24, &data[0]));
	
	mTextures[fileNameStr] = texture;
	return texture;
	
}

ls_Smart(ls::Scene) ls::ResourceManager::createSceneObj()
{
	return ls_Smart(ls::Scene)(new ls::Scene());
}

ls::CameraPtr ls::ResourceManager::createCamera(ParamSet & paramSet)
{
	ls_Assert(paramSet.getType() == "camera");

	CameraPtr camera = nullptr;

	if (paramSet.getName() == "pinhole" || paramSet.getName() == "pinholeCamera")
	{
		camera = new Pinhole(paramSet);
	}


	if(camera)
		mModules.push_back(camera);

	return camera;
}

ls::ScatteringFunctionPtr ls::ResourceManager::createScatteringFunction(ParamSet & paramSet)
{
	ls_Assert(paramSet.getType() == "scatteringFunction");

	ScatteringFunctionPtr scatter = nullptr;

	if (paramSet.getName() == "lambertian")
	{
		scatter = new Lambertian();
	}

	if (scatter)
		mModules.push_back(scatter);

	return scatter;
}

ls::FilmPtr ls::ResourceManager::createFilm(ParamSet & paramSet)
{
	ls_Assert(paramSet.getType() == "film");
	
	FilmPtr film = nullptr;
	
	if (paramSet.getName() == "hdr")
	{
		film = new HDRFilm(paramSet);
	}

	if (film)
		mModules.push_back(film);

	return film;
}

ls::LightPtr ls::ResourceManager::createLight(ParamSet & paramSet)
{
	ls_Assert(paramSet.getType() == "light");
	LightPtr light = nullptr;

	if (paramSet.getName() == "pointLight" || paramSet.getName() == "point")
	{
		light = new PointLight(paramSet);
	}
	if (light)
		mModules.push_back(light);

	return light;
}

ls::MaterialPtr ls::ResourceManager::createMaterial(ParamSet & paramSet)
{
	ls_Assert(paramSet.getType() == "material");

	MaterialPtr material = nullptr;

	if (paramSet.getName() == "matte")
	{
		material = new Matte(paramSet);
	}
	if (material)
		mModules.push_back(material);

	return material;
}

ls::SamplerPtr ls::ResourceManager::createSampler(ParamSet & paramSet)
{
	ls_Assert(paramSet.getType() == "sampler");

	SamplerPtr sampler = nullptr;

	sampler = new RandomSampler();

	if (sampler)
		mModules.push_back(sampler);

	return sampler;

}

ls::TexturePtr ls::ResourceManager::createTexture(ParamSet & paramSet)
{
	ls_Assert(paramSet.getType() == "texture");

	TexturePtr texture = nullptr;

	if (paramSet.getName() == "constantTexture" || paramSet.getName() == "constant")
	{
		texture = new ConstantTexture(paramSet);
	}
	else if (paramSet.getName() == "imageTexture" || paramSet.getName() == "image")
	{
		texture = new ImageTexture(paramSet);
	}

	if (texture)
		mModules.push_back(texture);

	return texture;
	
}

ls::MeshPtr ls::ResourceManager::createMesh(ParamSet & paramSet)
{
	return MeshPtr();
}



void ls::ResourceManager::write2File(ls::Texture * texture,
	const Path& path,
	const std::string& fileName)
{
	auto fullPath = Path(path.str() + "\\" + fileName);

	auto ext = fullPath.extension();

	

	ls_AssertMsg(texture->getType() == ETexImage, "Invalid texture type, only image texture can be written to file!");

	ImageTexture* image = dynamic_cast<ImageTexture*>(texture);
	auto width = image->getWidth();
	auto height = image->getHeight();
	auto channelType = image->getChannelType();
	auto bpp = image->getBPP();
	Spectrum* data = (Spectrum*)image->getRawData();

	FIBITMAP* bitmap = FreeImage_Allocate(width,
		height, bpp);


	for (u32 h = 0; h < height; ++h)
	{
		for (u32 w = 0; w < width; ++w)
		{
			RGBQUAD rgbquad;
			f32 rgb[3];
			data[h * width + w].toRGB(rgb);

			rgbquad.rgbRed = std::min(s32(rgb[0] * 255.f),s32(255));
			rgbquad.rgbGreen = std::min(s32(rgb[1] * 255.f), s32(255));
			rgbquad.rgbBlue = std::min(s32(rgb[2] * 255.f), s32(255));

			ls_AssertMsg(FreeImage_SetPixelColor(bitmap, w, h, &rgbquad), "Invalid Set Pixel Color");
		}

	}
	FreeImage_FlipVertical(bitmap);

	if (ext == "png" || ext == "PNG")
		FreeImage_Save(FIF_PNG, bitmap, fullPath.str().c_str());
	else if (ext == "bmp" || ext == "BMP")
		FreeImage_Save(FIF_BMP, bitmap, fullPath.str().c_str());
	else if (ext == "jpeg" || ext == "JPEG")
		FreeImage_Save(FIF_JPEG, bitmap, fullPath.str().c_str());
	else
		ls_AssertMsg(false, "Invalid Image Type to write!");

	
	
	


	
}

void ls::ResourceManager::write2FileW(ls::Texture * texture, 
	const Path& path,
	const std::wstring& fileName)
{
	auto fileNameStr = ls::toString(fileName);
	write2File(texture, path, fileNameStr);

}
