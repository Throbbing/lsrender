#include<film/hdr.h>
#include<function/log.h>
#include<texture/imageTexture.h>
f32 toSRGB(f32 v)
{
	if (v < 0.0031308f)
		return 12.92f * v;

	return 1.055 * std::powf(v, 1.0 / 2.4) - 0.055;
}

void ls::HDRFilm::commit()
{
	if (mWidth < 0 || mHeight < 0)
		ls_AssertMsg(false, "Invalid args in HDRFilm");

	mRenderBuffer.clear();

	mRenderBuffer.resize(mWidth * mHeight);
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
	std::vector<Vec3> data;
	for (auto& p : mRenderBuffer)
	{
		if (!lsMath::closeZero(p.weight))
			p.color /= p.weight;
		f32 rgb[3]; p.color.toRGB(rgb);

		for (int i = 0; i < 3; ++i) rgb[i] = toSRGB(rgb[i]);


		data.push_back(Vec3(rgb[2],rgb[1],rgb[0]));

		p.color = Spectrum(rgb[0],rgb[1],rgb[2]);
	}

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
}

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
