#include <resource\xmlHelper.h>
#include<function\log.h>
#include<math\transform.h>
#include<sstream>
namespace ls
{

	ls::XMLPackage ls::XMLParser::loadXMLFromMTSFile(const std::string & path, const std::string & file)
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
			auto paramSet = parseParam(node);

			package.mParamSets.push_back(paramSet);

			node = node->NextSiblingElement();
		}


		for (u32 i = 0; i < package.mParamSets.size(); ++i)
		{
			if (package.mParamSets[i].type == "integrator")
				package.mIntegrator = i;
			else if (package.mParamSets[i].type == "shape")
				package.mShapes[package.mParamSets[i].queryString("filename")] = i;
			else if (package.mParamSets[i].type == "bsdf")
				package.mBSDFs[package.mParamSets[i].id] = i;
			else if (package.mParamSets[i].type == "sensor")
				package.mCamera = i;
			else if (package.mParamSets[i].type == "medium")
				package.mMedium[package.mParamSets[i].id] = i;
		}
		

		return package;
	}

	s32 ls::XMLParser::parses32(tinyxml2::XMLElement * elem)
	{
		const char* value = "failed";
		elem->QueryStringAttribute("value", &value);

		std::stringstream ss;
		ss << value;
		s32 v;
		ss >> v;
		return v;
	}

	f32 ls::XMLParser::parsef32(tinyxml2::XMLElement * elem)
	{
		const char* value = "failed";
		elem->QueryStringAttribute("value", &value);

		std::stringstream ss;
		ss << value;
		f32 v;
		ss >> v;
		return v;
	}

	bool XMLParser::parseBool(tinyxml2::XMLElement * elem)
	{
		const char* value = "failed";
		elem->QueryStringAttribute("value", &value);

		if (std::string(value) == "true")
			return true;
		else
			return false;
	}

	std::string ls::XMLParser::parseString(tinyxml2::XMLElement * elem)
	{
		const char* value = "failed";
		elem->QueryStringAttribute("value", &value);
		return std::string(value);
	}

	Spectrum ls::XMLParser::parseColor(tinyxml2::XMLElement * elem)
	{
		const char* value = "failed";
		elem->QueryStringAttribute("value", &value);
		std::string str(value);
		if (str.empty())
			assert(false);
		for (auto& p : str)
			if (p == ',')
				p = ' ';
		std::stringstream ss;
		ss << str;
		f32 rgb[3];
		f32 t;
		int cCount = 0;
		while (ss >> t)
		{
			rgb[cCount++] = t;
		}
		if (cCount == 1)
			return Spectrum(rgb[0]);


		return Spectrum(rgb[0], rgb[1], rgb[2]);
	}

	Vec3 ls::XMLParser::parseVec3(tinyxml2::XMLElement * elem)
	{
		const char* x, *y, *z;
		ls_AssertMsg(XMLOK(elem->QueryStringAttribute("x", &x)), "Invalid X component in vector!");
		ls_AssertMsg(XMLOK(elem->QueryStringAttribute("y", &y)), "Invalid Y component in vector!");
		ls_AssertMsg(XMLOK(elem->QueryStringAttribute("z", &z)), "Invalid Z component in vector!");

		std::stringstream ss;
		ss << x << " " << y << " " << z << " ";

		Vec3 vec;
		ss >> vec.x >> vec.y >> vec.z;
		return vec;
	}

	Vec3 XMLParser::parseVec3(const char * v)
	{
		std::string str(v);
		for (auto& p : str)
			if (p == ',')
				p = ' ';

		Vec3 vec;
		std::stringstream ss;
		ss << str;
		ss >> vec.x >> vec.y >> vec.z;
		return vec;
	}

	Point3 ls::XMLParser::parsePoint(tinyxml2::XMLElement * elem)
	{
		const char* x, *y, *z;
		ls_AssertMsg(XMLOK(elem->QueryStringAttribute("x", &x)), "Invalid X component in point!");
		ls_AssertMsg(XMLOK(elem->QueryStringAttribute("y", &y)), "Invalid Y component in point!");
		ls_AssertMsg(XMLOK(elem->QueryStringAttribute("z", &z)), "Invalid Z component in point!");

		std::stringstream ss;
		ss << x << " " << y << " " << z << " ";

		Point3 p;
		ss >> p.x >> p.y >> p.z;
		return p;
	}

	Transform XMLParser::parseTransform(tinyxml2::XMLElement * elem)
	{
		bool isExplicit = false;

		Transform transform;

		//显式 Transform
		auto matrixNode = elem->FirstChildElement("matrix");
		if (matrixNode)
		{
			isExplicit = true;
			
			const char* value = "v";
			ls_AssertMsg(XMLOK(matrixNode->QueryStringAttribute("value", &value)),"Invalid matrix in transform!");
			std::string str(value);
			for (auto& p : str)
				if (p == ',')
					p = ' ';

			Mat4x4 mat;
			std::stringstream ss;
			ss << str;
			f32 arr[16];
			for (int i = 0; i < 16; ++i)
				ss >> arr[i];
			mat = Mat4x4(arr[0], arr[1], arr[2], arr[3],
				arr[4], arr[5], arr[6], arr[7],
				arr[8], arr[9], arr[10], arr[11],
				arr[12], arr[13], arr[14], arr[15]);

			transform = Transform(mat);
			return transform.getMat().transpose();
		}

		//拼接形 Transform
		auto translate = elem->FirstChildElement("translate");
		auto rotate = elem->FirstChildElement("rotate");
		auto scale = elem->FirstChildElement("scale");
		int jointCount = 0;
		Vec3 transVec(0), rotateVec(0), scaleVec(1);
		f32 rotateAngle;
		if (translate)
		{
			transVec = parseVec3(translate);
			jointCount++;
		}
		if (rotate)
		{
			const char* x = "0", *y = "0", *z = "0";
			const char* angle = "0";
			rotate->QueryStringAttribute("x", &x);
			rotate->QueryStringAttribute("y", &y);
			rotate->QueryStringAttribute("z", &z);
			rotate->QueryStringAttribute("angle", &angle);
			std::stringstream ss;
			ss << x << " " << y << " " << z << " " << angle;
			ss >> rotateVec.x >> rotateVec.y >> rotateVec.z >> rotateAngle;
			jointCount++;
		}
		if (scale)
		{
			const char *x = "1", *y = "1", *z = "1";
			if (XMLOK(scale->QueryStringAttribute("value", &x)))
			{
				f32 v = parsef32(scale);
				scaleVec = Vec3(v);
			}
			else
			{
				const char* x = "1", *y = "1", *z = "1";
				scale->QueryStringAttribute("x", &x);
				scale->QueryStringAttribute("y", &y);
				scale->QueryStringAttribute("z", &z);
				std::stringstream ss;
				ss << x << " " << y << " " << z;
				ss >> scaleVec.x >> scaleVec.y >> scaleVec.z;
			}
			jointCount++;
		}

		if (jointCount > 0)
		{
			transform = Transform::Mat4x4Scale(scaleVec) *
				Transform::Mat4x4Rotate(lsMath::degree2Radian(rotateAngle), rotateVec) *
				Transform::Mat4x4Translate(transVec);

			return transform;
		}


		//Lookat形 Transform
		
		{
			auto lookat = elem->FirstChildElement("lookat");
			if (!lookat)
				ls_AssertMsg(false, "Invalid Transform !!!!");

			Vec3 origin(0), target(0), up(0, 1, 0);

			const char* v = "failed";
			ls_AssertMsg(XMLOK(lookat->QueryStringAttribute("origin",&v)), "Lack Origin Attribute In Lookat ");
			origin = parseVec3(v);
			ls_AssertMsg(XMLOK(lookat->QueryStringAttribute("target", &v)), "Lack target Attribute In Lookat ");
			target = parseVec3(v);
			ls_AssertMsg(XMLOK(lookat->QueryStringAttribute("up", &v)), "Lack Up Attribute In Lookat ");
			up = parseVec3(v);

			transform = Transform::Mat4x4Camera(normalize(target - origin),
				up,
				Point3(origin));

			return transform;
		}


	}

	ParamSet ls::XMLParser::parseParam(tinyxml2::XMLElement * node)
	{
		//获取Node的信息
		auto paramSetTypeName = std::string(node->Name());
		const char* name = "failed";
		const char* varName = "";
		const char* id = "";
		node->QueryStringAttribute("type", &name);
		node->QueryStringAttribute("name", &varName);
		node->QueryStringAttribute("id", &id);

		if (std::string(id).empty())
			id = name;

		if (std::string(varName).empty())
			varName = name;

		ParamSet paramSet(paramSetTypeName, name,varName, id);

		auto attri = node->FirstChildElement();

		//遍历该Node中所有属性
		//并存放在ParamSet中
		while (attri)
		{
			auto nodeTypeName = std::string(attri->Name());
			attri->QueryStringAttribute("name", &name);

			//数值属性
			if (nodeTypeName == "integer")
			{
				auto v = parses32(attri);
				paramSet.adds32(name, v);
			}
			else if (nodeTypeName == "float")
			{
				auto v = parsef32(attri);
				paramSet.addf32(name, v);
			}
			else if (nodeTypeName == "boolean")
			{
				auto v = parseBool(attri);
				paramSet.addbool(name,v);
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
			else if (nodeTypeName == "point")
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
			else if (nodeTypeName == "ref")
			{
				const char* id = "id";
				attri->QueryStringAttribute("id", &id);

				paramSet.addRef(id, id);
			}
			else
			{
				auto v = parseParam(attri);
				paramSet.addParamSet(name, v);
			}

			attri = attri->NextSiblingElement();
		}

		return paramSet;
	}

	std::string XMLParser::parseRef(tinyxml2::XMLElement * elem)
	{
		return parseString(elem);
	}

	void XMLParser::printXMLPackage(XMLPackage & xmlPackage)
	{
		for (auto& p : xmlPackage.mParamSets)
		{
			printParamSet(p, 0);
		}

	}

	void XMLParser::printParamSet(ParamSet & paramSet, int depth)
	{
		std::string indent="";
		for (int i = 0; i < depth; ++i)
		{
			indent += "  ";
		}

		std::cout <<indent<< "Begin type = " << paramSet.type << " name =  " << paramSet.name <<" varname = "<<paramSet.varName<< std::endl;

		for (auto p : paramSet.f32s)
			std::cout << indent << " " << "Type = f32" << " name = " << p.first << " value = " << p.second << std::endl;

		for (auto p : paramSet.s32s)
			std::cout << indent << " " << "Type = s32" << " name = " << p.first << " value = " << p.second << std::endl;

		for(auto p :paramSet.bools)
			std::cout << indent << " " << "Type = boolean" << " name = " << p.first << " value = " << p.second << std::endl;

		for(auto p : paramSet.strings)
			std::cout << indent << " " << "Type = string" << " name = " << p.first << " value = " << p.second << std::endl;

		for (auto p : paramSet.Spectrums)
			std::cout << indent << " " << "Type = Spectrum" << " name = " << p.first << " value = ( " << p.second[0] << " " << p.second[1] << " " << p.second[2] << " ) " << std::endl;

		for(auto p : paramSet.Vec3s)
			std::cout << indent << " " << "Type = Vec3" << " name = " << p.first << " value = ( " << p.second[0] << " " << p.second[1] << " " << p.second[2] << " ) " << std::endl;
	
		for(auto p : paramSet.refs)
			std::cout << indent << " " << "Type = ref" << " name = " << p.first << " value = " << p.second << std::endl;

		for (auto p : paramSet.Transforms)
		{
			std::cout << indent << " " << "Type = Transform" << " name = " << p.first << " value = ( " << std::endl;
			auto mat = p.second.getMat();
			for (int i = 0; i < 16; ++i)
			{
				std::cout << mat(i) << " ";
			}
			std::cout << " ) " << std::endl;
		}

		for (auto& p : paramSet.childParamSets)
			printParamSet(p, depth + 1);

		std::cout << indent << "End " << std::endl;
	}

}