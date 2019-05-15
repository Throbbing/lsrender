#include <texture/imageTexture.h>
#include <function/log.h>
#include<record/record.h>
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

ls::Spectrum ls::ImageTexture::fetch(ls_Param_In const IntersectionRecord * rec)
{
	Unimplement;
}


