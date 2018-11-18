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
		
		std::vector<ParamSet>		ParamSets;

		
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

	};
}