#pragma once
#include<config/config.h>
#include<config/declaration.h>
#include<config/module.h>
#include<spectrum/spectrum.h>
namespace ls
{
	enum TextureChannelType
	{
		ERGB = 0,
		ERGBA,
		EGBR,
		EGBRA,
		ESpectrum,
		ETextureChannelTypeCount
	};

	enum TextureType
	{
		ETexConstant = 0,
		ETexImage,
		ETextureTypeCount
	};
	class Texture:public Module
	{
		friend Scene;
	public:
		Texture(TextureType type,
			const std::string& id = "texture"):Module(id),mTextureType(type){}
		virtual ~Texture() {}

		virtual TextureType getType() const { return mTextureType; }

		virtual u32 getWidth()			 const		{ return 1; }
		virtual u32 getHeight()			 const		{ return 1; }
		virtual void* getRawData()					{ return nullptr; }
		virtual const void* getRawData() const		{ return nullptr; }

		virtual void setRes(u32 w, u32 h) {};
		virtual void	 commit() = 0;
		virtual Spectrum fetch(ls_Param_In const IntersectionRecord* rec) = 0;
	
	protected:
		TextureType				mTextureType;
	};
}