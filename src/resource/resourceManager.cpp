#include<resource/resourceManager.h>
#include<3rd/tiny_obj_loader.h>
#include<function/file.h>
#include<3rd/FreeImage.h>
#include<function/log.h>

#include<algorithm/direct.h>
#include<algorithm/path.h>
#include<algorithm/bdpt.h>
#include<camera/pinhole.h>

#include<film/hdr.h>
#include<light/pointLight.h>
#include<light/environmentLight.h>
#include<light/areaLight.h>

#include<material/glass.h>
#include<material/matte.h>
#include<material/specularMetal.h>
#include<material/glossyMetal.h>

#include<mesh/mesh.h>
#include<mesh/trimesh.h>
#include<sampler/randomsampler.h>
#include<scatter/lambertian.h>
#include<scatter/dielectric.h>
#include<scatter/conductor.h>
#include<scatter/microfacetConductor.h>
#include<scene/scene.h>

#include<resource/xmlHelper.h>
#include<texture/constantTexture.h>
#include<texture/imageTexture.h>
#include<thread/QueuedThreadPool.h>



#include<cstring>
#include<windows.h>


std::map<std::string, ls::ResourceManager::MeshIndex> ls::ResourceManager::mMeshIndices;
std::vector<ls::MeshPtr>	ls::ResourceManager::mMeshs;
std::map<std::string, ls::ImageData> ls::ResourceManager::mImageDatas;
std::vector<ls::ModulePtr> ls::ResourceManager::mModules;
std::string					ls::ResourceManager::mPath;
std::vector<ls::QueuedThreadPoolPtr> ls::ResourceManager::mThreadPools;

void ls::ResourceManager::clear()
{
	for (auto& p : mModules)
	{
		ReleaselsPtr(p);
	}
	mModules.clear();

	for (auto& p : mThreadPools)
	{
		ReleaselsPtr(p);
	}
	mThreadPools.clear();
}

std::vector<ls::MeshPtr> ls::ResourceManager::loadMeshFromFile(Path fullPath)
{
	if (!fullPath.isAbsolute())
	{
		fullPath = mPath + fullPath.str();
	}
	auto fileName = fullPath.filename();
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
		&materials, &err,&warn, fullPath.str().c_str(),nullptr,true))
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
		mModules.push_back(mMeshs[i]);
	}
	
	return meshs;
}


ls::ImageData ls::ResourceManager::loadTextureFromFile(Path  fullPath)
{
	if (!fullPath.isAbsolute())
	{
		fullPath = mPath + fullPath.str();
	}
	std::string fileNameStr = fullPath.filename();
	if (mImageDatas.find(fileNameStr) != mImageDatas.end())
		return mImageDatas[fileNameStr];

	
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
//	FreeImage_FlipVertical(dib);

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
		if (bpp != 24)
			dib = FreeImage_ConvertTo24Bits(dib);
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

#ifdef ls_GAMMA_CORRECTION
				r = std::powf(r, 2.2f);
				g = std::powf(g, 2.2f);
				b = std::powf(b, 2.2f);
#endif // ls_GAMMA_CORRECTION


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

	ImageData imageData;
	imageData.width = width;
	imageData.height = height;
	imageData.data = std::move(data);
	
	mImageDatas[fileNameStr] = imageData;

	return imageData;
}


ls::ScenePtr ls::ResourceManager::createSceneObj()
{
	if (lsRender::scene)
		return lsRender::scene;

	lsRender::scene = new Scene();

	mModules.push_back(lsRender::scene);

	return lsRender::scene;
}

ls::RenderAlgorithmPtr ls::ResourceManager::createAlgorithm(ParamSet & paramSet)
{
	ls_Assert(paramSet.getClass() == "renderAlgorithm");

	RenderAlgorithmPtr renderAlgorithm = nullptr;

	if (paramSet.getType() == "direct")
	{
		renderAlgorithm = new DirectTracer(paramSet);
	}
	else if (paramSet.getType() == "path")
	{
		renderAlgorithm = new PathTracer(paramSet);
	}
	else if (paramSet.getType() == "bdpt")
	{
		renderAlgorithm = new BDPT(paramSet);
	}

	if (renderAlgorithm)
		mModules.push_back(renderAlgorithm);

	return renderAlgorithm;
}

ls::CameraPtr ls::ResourceManager::createCamera(ParamSet & paramSet)
{
	ls_Assert(paramSet.getClass() == "camera");

	CameraPtr camera = nullptr;

	if (paramSet.getType() == "pinhole" || paramSet.getType() == "pinholeCamera")
	{
		camera = new Pinhole(paramSet);
		auto films = paramSet.queryParamSetByClass("film");

		if(!films.empty())
			camera->addFilm(createFilm(films[0]));
	}


	if(camera)
		mModules.push_back(camera);

	return camera;
}

ls::ScatteringFunctionPtr ls::ResourceManager::createScatteringFunction(ParamSet & paramSet)
{
	ls_Assert(paramSet.getClass() == "scatteringFunction");

	ScatteringFunctionPtr scatter = nullptr;

	if (paramSet.getType() == "lambertian")
	{
		scatter = new Lambertian();
	}
	else if (paramSet.getType() == "dielectric")
	{
		
		scatter = new Dielectric(paramSet);
	}
	else if (paramSet.getType() == "conductor")
	{
		scatter = new Conductor(paramSet);
	}
	else if (paramSet.getType() == "microfacetConductor")
	{
		scatter = new MicrofacetConductor(paramSet);
	}
	

	if (scatter)
		mModules.push_back(scatter);

	return scatter;
}

ls::FilmPtr ls::ResourceManager::createFilm(ParamSet & paramSet)
{
	ls_Assert(paramSet.getClass() == "film");
	
	FilmPtr film = nullptr;
	
	if (paramSet.getType() == "hdr")
	{
		film = new HDRFilm(paramSet);
	}

	if (film)
		mModules.push_back(film);

	return film;
}

ls::LightPtr ls::ResourceManager::createLight(ParamSet & paramSet)
{
	ls_Assert(paramSet.getClass() == "light");
	LightPtr light = nullptr;

	if (paramSet.getType() == "pointLight" || paramSet.getType() == "point")
	{
		light = new PointLight(paramSet);
	}
	else if (paramSet.getType() == "environmentLight" || paramSet.getType() == "environment")
	{
		light = new EnvironmentLight(paramSet);
	}
	else if (paramSet.getType() == "areaLight" || paramSet.getType() == "area")
	{
		light = new AreaLight(paramSet);
	}
	else
	{
		auto t = paramSet.getType() + " in mitsuba has not been supported in lsrender!";
		ls_AssertMsg(false, t);
	}
	if (light)
		mModules.push_back(light);

	return light;
}

ls::MaterialPtr ls::ResourceManager::createMaterial(ParamSet & paramSet)
{
	ls_Assert(paramSet.getClass() == "material");

	MaterialPtr material = nullptr;

	if (paramSet.getType() == "matte")
	{
		material = new Matte(paramSet);
	}
	else if (paramSet.getType() == "glass")
	{
		material = new Glass(paramSet);
	}
	else if (paramSet.getType() == "specularMetal")
	{
		material = new SpecularMetal(paramSet);
	}
	else if (paramSet.getType() == "glossyMetal")
	{
		material = new GlossyMetal(paramSet);
	}
	else
	{
		auto t = paramSet.getType() + " in mitsuba has not been supported in lsrender!";
		ls_AssertMsg(false, t);
	}
	if (material)
		mModules.push_back(material);

	return material;
}

ls::SamplerPtr ls::ResourceManager::createSampler(ParamSet & paramSet)
{
	ls_Assert(paramSet.getClass() == "sampler");

	SamplerPtr sampler = nullptr;

	sampler = new RandomSampler();

	if (sampler)
		mModules.push_back(sampler);

	return sampler;

}

ls::TexturePtr ls::ResourceManager::createTexture(ParamSet & paramSet)
{
	ls_Assert(paramSet.getClass() == "texture");

	TexturePtr texture = nullptr;

	if (paramSet.getType() == "constantTexture" || paramSet.getType() == "constant")
	{
		texture = new ConstantTexture(paramSet);
	}
	else if (paramSet.getType() == "imageTexture" || paramSet.getType() == "image")
	{
		texture = new ImageTexture(paramSet);
	}

	if (texture)
		mModules.push_back(texture);

	return texture;
	
}

std::vector<ls::MeshPtr> ls::ResourceManager::createMesh(ParamSet & paramSet)
{
	ls_Assert(paramSet.getClass() == "mesh");

	MeshPtr mesh = nullptr;

	if (paramSet.getType() == "triMesh" || paramSet.getType() == "tri")
	{
		auto fullPath = paramSet.queryString("filename");
		auto t = loadMeshFromFile(fullPath);

		for (auto& p : t)
		{
			p->applyMaterial(createMaterial(paramSet.queryParamSetByClass("material")[0]));
			p->applyTransform(paramSet.queryTransform("world"));
			
			auto areas = paramSet.queryParamSetByClass("light");
			if (!areas.empty())
			{
				auto radiance = areas[0].querySpectrum("radiance", 1.f);
				p->applyAreaLight(radiance);
			}
		}

		return t;
	}
	else if (paramSet.getType() == "rectangle" || paramSet.getType() == "rect")
	{
		auto p = createRectangleMesh(paramSet);
		std::vector<ls::MeshPtr> meshs;
		meshs.push_back(p);
		return meshs;
	}
	else
	{
		auto t = paramSet.getType() + " in mitsuba has not been supported in lsrender!";
		ls_AssertMsg(false, t);
	}

	return std::vector<ls::MeshPtr>();



}

ls::MeshPtr ls::ResourceManager::createRectangleMesh(ParamSet & paramSet)
{
	std::vector<Point3> positions;
	std::vector<Normal> normals;
	std::vector<Point2>	texs;
	std::vector<u32>	indices;

	auto world = paramSet.queryTransform("world");


	positions.push_back(Point3(-1, -1, 0));
	positions.push_back(Point3(-1, 1, 0));
	positions.push_back(Point3(1, 1, 0));

	positions.push_back(Point3(1, 1, 0));
	positions.push_back(Point3(1, -1, 0));
	positions.push_back(Point3(-1, -1, 0));

	normals.push_back(Normal(0, 0, 1));
	normals.push_back(Normal(0, 0, 1));
	normals.push_back(Normal(0, 0, 1));
	normals.push_back(Normal(0, 0, 1));
	normals.push_back(Normal(0, 0, 1));
	normals.push_back(Normal(0, 0, 1));

	texs.push_back(Point2(0, 0));
	texs.push_back(Point2(0, 1));
	texs.push_back(Point2(1, 1));
	texs.push_back(Point2(1, 1));
	texs.push_back(Point2(1, 0));
	texs.push_back(Point2(0, 0));

	indices.push_back(0); indices.push_back(1); indices.push_back(2);
	indices.push_back(3); indices.push_back(4); indices.push_back(5);

	MeshPtr p = new TriMesh(positions, normals, texs, indices);
	
	p->applyMaterial(createMaterial(paramSet.queryParamSetByClass("material")[0]));
	p->applyTransform(world);

	auto areas = paramSet.queryParamSetByClass("light");
	if (!areas.empty())
	{
		auto radiance = areas[0].querySpectrum("radiance", 1.f);
		p->applyAreaLight(radiance);
	}

	mModules.push_back(p);
	return p;
}



void ls::ResourceManager::write2File(ls::Texture * texture,
	const Path& fullPath)
{
	
	auto ext = fullPath.extension();

	

	ls_AssertMsg(texture->getType() == ETexImage, "Invalid texture type, only image texture can be written to file!");

	ImageTexture* image = dynamic_cast<ImageTexture*>(texture);
	auto width = image->getWidth();
	auto height = image->getHeight();
	auto channelType = image->getChannelType();
	auto bpp = 24;
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

ls::QueuedThreadPoolPtr ls::ResourceManager::createThreadPool(s32 threadCount)
{
	auto pool = new QueuedThreadPool(threadCount);
	mThreadPools.push_back(pool);
	return pool;
}


