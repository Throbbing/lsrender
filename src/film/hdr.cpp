#include<film/hdr.h>
#include<function/log.h>
#include<texture/imageTexture.h>
#include<resource/xmlHelper.h>
#include<resource/resourceManager.h>


ls::FilmPtr ls::HDRFilm::copy() const
{
	ParamSet paramSet = ParamSet("film", "hdr", "hdr", "hdr");
	paramSet.adds32("width", mWidth);
	paramSet.adds32("height", mHeight);
	return ResourceManager::createFilm(paramSet);
}

void ls::HDRFilm::merge(const std::vector<FilmPtr>& films)
{
	for (auto p : films)
	{
		if (typeid(*p) != typeid(HDRFilm))
			continue;

		HDRFilm* film = dynamic_cast<HDRFilm*>(p);

		if (film->mRenderBuffer.size() != mRenderBuffer.size())
			continue;

		for (s32 i = 0; i < mRenderBuffer.size(); ++i)
		{
			auto& pixel = mRenderBuffer[i];
			auto& filmPixel = film->mRenderBuffer[i];

			pixel.color += filmPixel.color;
			pixel.weight += filmPixel.weight;
		}
	}
}

void ls::HDRFilm::commit()
{
	if (mWidth < 0 || mHeight < 0)
		ls_AssertMsg(false, "Invalid args in HDRFilm");

	mRenderBuffer.clear();

	mRenderBuffer.resize(mWidth * mHeight);
}

std::string ls::HDRFilm::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Film: " << "HDR" << std::endl;
	oss << "Width: " << mWidth << std::endl;
	oss << "Height: " << mHeight << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

void ls::HDRFilm::addPixel(const Spectrum& color,
	float xpos, float ypos)
{

	s32 x = lsMath::clamp(s32(xpos), 0, mWidth - 1);
	s32 y = lsMath::clamp(s32(ypos), 0, mHeight - 1);

	auto& pixel = mRenderBuffer[y * mWidth + x];
	pixel.x = x;
	pixel.y = y;
	pixel.color += color;
	pixel.weight += 1.f;
	
}

void ls::HDRFilm::flush()
{
	int index = 0;

#ifdef ls_OPENMP
#pragma omp parallel for
#endif
	for (int i = 0; i < mRenderBuffer.size(); ++i)
	{
		auto& p = mRenderBuffer[i];

		if (!lsMath::closeZero(p.weight))
			p.color /= p.weight;
		f32 rgb[3]; p.color.toRGB(rgb);

		for (int i = 0; i < 3; ++i) rgb[i] = toSRGB(rgb[i]);



		p.color = Spectrum(rgb[0], rgb[1], rgb[2]);
	}
}

#if 0
	std::ofstream file;
	file.open("Test.ppm", std::ios::out);

	if (file.fail())
	{
		std::cout << "Êä³ö´ò¿ªÊ§°Ü" << std::endl;
		system("pause");
	}



	file << "P3" << std::endl;
	file << mWidth << " " << mHeight << std::endl;
	file << 255 << std::endl;
	u32 r, g, b;
	Spectrum color;
	f32 rgb[3];
	for (s32 y = 0; y < mHeight; ++y)
	{

		for (s32 x = 0; x < mWidth; ++x)
		{

			color = mRenderBuffer[y *mWidth + x].color;
			color.toRGB(rgb);
			file << lsMath::clamp(s32(rgb[0] * 255.f), 0, 255) << ' ' <<
				lsMath::clamp(s32(rgb[1] * 255.f), 0, 255) << ' ' <<
				lsMath::clamp(s32(rgb[2] * 255.f), 0, 255) << ' ';

		}
		file << std::endl;

	}
	file.close();
#endif


ls::TexturePtr ls::HDRFilm::convert2Texture() const
{
	ImageTexture* image = new ImageTexture(mWidth, mHeight);

	std::vector<Spectrum> data;
	for (auto& pixel : mRenderBuffer)
	{
		data.push_back(pixel.color);
	}

	image->setData(&data[0]);
	image->commit();
	return image;
}

ls::HDRFilm::HDRFilm(ParamSet & paramSet)
{
	mWidth = paramSet.querys32("width");
	mHeight = paramSet.querys32("height");


}
