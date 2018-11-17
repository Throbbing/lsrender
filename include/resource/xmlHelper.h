#pragma once
#include<config\config.h>
#include<config\declaration.h>
#include<resource\paramSet.h>
#include<3rd\tinyxml2.h>


namespace ls
{
	class XMLPackage
	{
	public:
		XMLPackage() {}
		
		std::vector<ParamSet>		ParamSets;

		
	};


	class XMLParser
	{
	public:
		static XMLPackage loadXMLFromFile(const std::string &path,
			const std::string& file);

		static int				parseInt(tinyxml2::XMLElement* elem);
		static f32				parsef32(tinyxml2::XMLElement* elem);
		static std::string		parseString(tinyxml2::XMLElement* elem);
		static Spectrum			parseColor(tinyxml2::XMLElement* elem);
		static Vec3				parseVec3(tinyxml2::XMLElement* elem);
		static Point3			parsePoint(tinyxml2::XMLElement* elem);
		static Transform		parseTransform(tinyxml2::XMLElement* elem);
		static ParamSet			parseParam(tinyxml2::XMLElement* elem);
		static std::string      parseRef(tinyxml2::XMLElement* elem);
	};
}