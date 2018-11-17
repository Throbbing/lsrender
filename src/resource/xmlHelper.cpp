#include <resource\xmlHelper.h>

namespace ls
{

	ls::XMLPackage ls::XMLParser::loadXMLFromFile(const std::string & path, const std::string & file)
	{
		tinyxml2::XMLDocument doc;
		doc.LoadFile((path + file).c_str());

		//得到场景节点
		auto sc = doc.FirstChildElement();

		if (!sc)
			return XMLPackage();

		auto node = sc->FirstChildElement();
		XMLPackage package;

		//mts的plugin循环
		while (node)
		{
			auto paramSetTypeName = std::string(node->Name);
			char* name = "failed";
			node->QueryStringAttribute("type", &name);

			ParamSet paramSet(paramSetTypeName, name);

			auto attri = node->FirstChildElement();
			//创建该Plugin
			while (attri)
			{
				auto nodeTypeName = std::string(attri->Name());
				attri->QueryStringAttribute("name", &name);

				if (nodeTypeName == "integer")
				{
					auto v = parseInt(attri);
					paramSet.adds32(name, v);
				}
				else if (nodeTypeName == "float")
				{
					auto v = parsef32(attri);
					paramSet.addf32(name, v);
				}
				else if (nodeTypeName == "string")
				{
					auto v = parseString(attri);
					paramSet.addString(name, v);
				}
				else if (nodeTypeName == "rgb")
				{
					auto v = parseColor(attri);
					paramSet.addSpectrum(name, v);
				}
				else if (nodeTypeName == "spectrum")
				{
					auto v = parseColor(attri);
					paramSet.addSpectrum(name, v);
				}
				else if (nodeTypeName == "Point")
				{
					auto v = parseVec3(attri);
					paramSet.addVec3(name, v);
				}
				else if (nodeTypeName == "vector")
				{
					auto v = parseVec3(attri);
					paramSet.addVec3(name, v);
				}
				else if (nodeTypeName == "transform")
				{
					auto v = parseTransform(attri);
					paramSet.addTransoform(name, v);
				}
				else if(nodeTypeName == "ref")
				{
					auto v = parseRef(attri);
					char* id = "id";
					attri->QueryStringAttribute("id", &id);

					paramSet.addRef(id, v);
				}
				else
				{
					auto v = parseParam(attri);
					paramSet.addParamSet(name, v);
				}

				attri = attri->NextSiblingElement();
			}
			
			node = node->NextSiblingElement();
			package.ParamSets.push_back(paramSet);
			
		}
		

		return package;
	}

	int ls::XMLParser::parseInt(tinyxml2::XMLElement * elem)
	{
		return 0;
	}

	f32 ls::XMLParser::parsef32(tinyxml2::XMLElement * elem)
	{
		return f32();
	}

	std::string ls::XMLParser::parseString(tinyxml2::XMLElement * elem)
	{
		return std::string();
	}

	Spectrum ls::XMLParser::parseColor(tinyxml2::XMLElement * elem)
	{
		return Spectrum();
	}

	Vec3 ls::XMLParser::parseVec3(tinyxml2::XMLElement * elem)
	{
		return Vec3();
	}

	Point3 ls::XMLParser::parsePoint(tinyxml2::XMLElement * elem)
	{
		return Point3();
	}

	ParamSet ls::XMLParser::parseParam(tinyxml2::XMLElement * elem)
	{
		return ParamSet();
	}
}