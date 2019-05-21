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

ls::Spectrum ls::ImageTexture::fetch(ls_Param_In const IntersectionRecord * rec)
{
	Unimplement;
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


