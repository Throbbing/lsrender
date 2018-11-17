#pragma once
#include<config\config.h>
#include<config\declaration.h>




namespace ls
{
	class XMLPackage
	{
	public:
		XMLPackage() {}
		

	};


	class XMLParser
	{
	public:
		static XMLPackage loadXMLFromFile(const std::string &path,
			const std::string& file);
	};
}