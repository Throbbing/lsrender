#pragma once
#include<config\config.h>
#include<config\declaration.h>
#include<resource\paramSet.h>
#include<3rd\tinyxml2.h>


namespace ls
{
#define XMLOK(p) p == tinyxml2::XMLError::XML_SUCCESS
	class XMLPackage
	{
	public:
		XMLPackage() {}
		
		std::vector<ParamSet>				mParamSets;

		ParamSet							mSampleInfo;
		u32									mIntegrator;
		u32									mCamera;
		std::map<std::string, size_t>		mShapes;
		std::map<std::string, size_t>		mBSDFs;
		std::map<std::string, size_t>		mMedium;
		std::map<std::string, size_t>		mLights;
		std::map<std::string, size_t>		mTextures;

		ParamSet		queryRefObject(const std::map<std::string, std::string>& refs,
			ParamSetType type);

		std::vector<ParamSet>	queryParamSetByClass(const std::string& varClass);
		std::vector<ParamSet>	queryParamSetByType(const std::string& varType);
		ParamSet				queryParamSetByName(const std::string& name);
	};

	

	class XMLParser
	{
		
	public:
		static XMLPackage loadXMLFromMTSFile(const std::string &path,
			const std::string& file);

		static void printXMLPackage(XMLPackage& xmlPackage);
	
		
	private:

		static s32				parses32(tinyxml2::XMLElement* elem);
		static f32				parsef32(tinyxml2::XMLElement* elem);
		static bool				parseBool(tinyxml2::XMLElement* elem);
		static std::string		parseString(tinyxml2::XMLElement* elem);
		static Spectrum			parseColor(tinyxml2::XMLElement* elem);
		static Vec3				parseVec3(tinyxml2::XMLElement* elem);
		static Vec3				parseVec3(const char* v);
		static Point3			parsePoint(tinyxml2::XMLElement* elem);
		static Transform		parseTransform(tinyxml2::XMLElement* elem);
		static ParamSet			parseParam(tinyxml2::XMLElement* elem);
		static std::string      parseRef(tinyxml2::XMLElement* elem);

		
		
		static void printParamSet(ParamSet& paramSet,int depth);


		//convert Mitsuba XMLPackage to lsrender XMLPackage
		static ParamSet			mts2lsCamera(XMLPackage& src, ParamSet& mtsCamera);
		static ParamSet			mts2lsAlgorithm(XMLPackage& src, ParamSet& mtsIntegrator);
		static ParamSet			mts2lsFilm(XMLPackage& src, ParamSet& mtsFilm);
		static ParamSet			mts2lsMaterial(XMLPackage& src, ParamSet& mtsBSDF);
		static ParamSet			mts2lsTextureParameter(XMLPackage& src, const std::string& parameter, ParamSet& param);
		static ParamSet			mts2lsTexture(XMLPackage& src, ParamSet& mtsTexture);
		static ParamSet			mts2lsTexture(XMLPackage& src, Spectrum mtsTexture,const std::string& name,const std::string& id);
		static ParamSet			mts2lsMesh(XMLPackage& src, ParamSet& mtsShape);
		static ParamSet			mts2lsSampler(XMLPackage& src, ParamSet& mtsSampler);
		static ParamSet			mts2lsLight(XMLPackage& src, ParamSet& mtsLight);
		static XMLPackage		mts2ls(XMLPackage src);
	};
}