#pragma once
#include<texture/texture.h>
#include<spectrum/spectrum.h>


namespace ls
{
	enum WrapMode
	{
		EWrap_Repeat = 0,
		EWrap_Clamp,
		EWrap_Mirror
	};

	struct ImageData
	{
	public:
		s32							width;
		s32							height;

		std::vector<Spectrum>		data;
	};
	class ImageTexture :public Texture
	{
		friend ResourceManager;
	public:
		ImageTexture():Texture(ETexImage){}
		ImageTexture(u32 width, u32 height,
			TextureChannelType channelType = ERGB,
			u32		bpp = 24,
			Spectrum* data = nullptr);
		virtual				~ImageTexture() {}
		

		virtual u32			getWidth()  const	{ return mWidth; }
		virtual u32			getHeight() const	{ return mHeight; }
		virtual void*		getRawData() override	{ return &mData[0]; }
		virtual const void* getRawData() const  override	{ return &mData[0]; }
		virtual TextureChannelType
			getChannelType() const { return mChannelType; }
		virtual	u32			getBPP() const { return mBPP;}



		virtual void		setRes(u32 w, u32 h) override;
		virtual void		setData(Spectrum* data);
		virtual void		commit() override;
		virtual std::string strOut() const override;
		virtual Spectrum	fetch(ls_Param_In const IntersectionRecord* rec) override;

		

		
	protected:
		ImageTexture(ParamSet& paramSet);

	protected:
		TextureChannelType			mChannelType;
		u32							mBPP;

		std::vector<Spectrum>		mData;
		u32							mWidth;
		u32							mHeight;
		WrapMode					mWrapU;
		WrapMode					mWrapV;
	};
}

