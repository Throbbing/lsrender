#include <texture/imageTexture.h>
#include <function/log.h>
#include<record/record.h>
#include<resource/xmlHelper.h>
#include<resource/resourceManager.h>
#include<function/file.h>
ls::ImageTexture::ImageTexture(u32 width, 
	u32 height,
	TextureChannelType channelType,
	u32 bpp,
	Spectrum * data):Texture(ETexImage)
{
	mChannelType = channelType;
	mBPP = bpp;
	mWidth = width;
	mHeight = height;

	
	if (data)
	{
		mData.resize(mWidth* mHeight);

		for (u32 h = 0; h < mHeight; ++h)
		{
			for (u32 w = 0; w < mWidth; ++w)
			{
				mData[h * mWidth + w] = data[h* mWidth + w];
			}
		}
	}
}

void ls::ImageTexture::setRes(u32 w, u32 h)
{
	mWidth = w;
	mHeight = h;
	mData.clear();
}

void ls::ImageTexture::setData(Spectrum * data)
{
	mData.clear();
	mData.resize(mWidth* mHeight);
	for (u32 h = 0; h < mHeight; ++h)
	{
		for (u32 w = 0; w < mWidth; ++w)
		{
			mData[h* mWidth + w] = data[h*mWidth + w];
		}
	}
}

void ls::ImageTexture::commit()
{
}

std::string ls::ImageTexture::strOut() const
{
	std::ostringstream oss;
	oss << ls_Separator << std::endl;
	oss << "Texture: " << "Image" << std::endl;
	oss << ls_Separator << std::endl;
	return oss.str();
}

f32 wrapModeFunction(f32 texCoord,
	f32 res,
	ls::WrapMode mode)
{
	if (mode == ls::WrapMode::EWrap_Clamp)
	{
		if (texCoord >= res)
			texCoord = res - 1;
		else if (texCoord < 0)
			texCoord = 0.f;
	}
	else if (mode == ls::WrapMode::EWrap_Repeat)
	{
		while (texCoord < 0.f)
			texCoord += res;
		while (texCoord >= res)
			texCoord -= res;
	}
	else if (mode == ls::WrapMode::EWrap_Mirror)
	{
		if (texCoord < 0.f)
		{
			while (texCoord < -res)
				texCoord += res;

			texCoord = -texCoord;
		}
		else if (texCoord >= res)
		{
			while (texCoord > 2 * res)
				texCoord -= res;

			texCoord = 2 * res - texCoord;

		}
	}
	return texCoord;
}


ls::Spectrum ls::ImageTexture::fetch(ls_Param_In const IntersectionRecord * rec)
{
	Vec2 uv = rec->uv;
	uv.x *= f32(mWidth);
	uv.y *= f32(mHeight);

	//根据Wrap Mode 对纹理坐标进行处理
	if (uv.x < 0.f || uv.x >= f32(mWidth)) uv.x = wrapModeFunction(uv.x, f32(mWidth), mWrapU);
	if (uv.y < 0.f || uv.y >= f32(mHeight)) uv.y = wrapModeFunction(uv.y, f32(mHeight), mWrapV);

#if 0
	//点采样
	{
		return mData[s32(uv.y) * mWidth + s32(uv.x)];
	}
#else

	//双线性插值
	{
		s32 u0 = s32(uv.x); s32 u1 = std::min(u0 + 1, s32(mWidth - 1));
		s32 v0 = s32(uv.y); s32 v1 = std::min(v0 + 1, s32(mHeight - 1));

		f32 uOffset = uv.x - u0;
		f32 vOffset = uv.y - v0;

		return (1.f - uOffset) * (1.f - vOffset) * mData[fetch(u0, v0)] +
			(uOffset) * (1.f - vOffset) *mData[fetch(u1, v0)] +
			(1.f - uOffset) * (vOffset)* mData[fetch(u0, v1)] +
			(uOffset) * (vOffset)* mData[fetch(u1, v1)];
	}
#endif
}

ls::ImageTexture::ImageTexture(ParamSet & paramSet):Texture(ETexImage)
{
	Path fullPath = paramSet.queryString("filename");



	auto imageData = ResourceManager::loadTextureFromFile(fullPath);

	mChannelType = ESpectrum;
	mBPP = 32;

	mWrapU = EWrap_Repeat;
	mWrapV = EWrap_Repeat;

	std::string mtsWrapModeU = paramSet.queryString("wrapModeU");
	std::string mtsWrapModeV = paramSet.queryString("wrapModeV");
	if (mtsWrapModeU == "repeat")
	{
		mWrapU = EWrap_Repeat;
	}
	else if (mtsWrapModeU == "mirror")
	{
		mWrapU = EWrap_Mirror;
	}
	else if (mtsWrapModeU == "clamp")
	{
		mWrapU = EWrap_Clamp;
	}


	if (mtsWrapModeV == "repeat")
	{
		mWrapV = EWrap_Repeat;
	}
	else if (mtsWrapModeV == "mirror")
	{
		mWrapV = EWrap_Mirror;
	}
	else if (mtsWrapModeV == "clamp")
	{
		mWrapV = EWrap_Clamp;
	}

	setRes(imageData.width, imageData.height);
	setData(&imageData.data[0]);

}


