#include<film\hdr.h>
#include<function\log.h>
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
	for (auto& p : mRenderBuffer)
	{
		if (!lsMath::closeZero(p.weight))
			p.color /= p.weight;
	}
}
