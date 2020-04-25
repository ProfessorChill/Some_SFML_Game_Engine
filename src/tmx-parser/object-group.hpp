#ifndef TMX_PARSER_OBJECT_GROUP_HPP
#define TMX_PARSER_OBJECT_GROUP_HPP

#include <string>
#include <tinyxml2.h>
#include <vector>

#include "object.hpp"

namespace tmx
{
class ObjectGroup
{
public:
	ObjectGroup(tinyxml2::XMLElement *objectGroupElement)
	{
		objectGroupElement->QueryUnsignedAttribute("id", &this->id);
		this->name = objectGroupElement->Attribute("name");

		tinyxml2::XMLElement *objListElement =
		    objectGroupElement->FirstChildElement("object");
		while (objListElement != nullptr) {
			this->objects.push_back(tmx::Object(objListElement));

			objListElement = objListElement->NextSiblingElement("object");
		}
	}

	unsigned int id;
	std::string name;

	std::vector<tmx::Object> objects;
};
} // namespace tmx

#endif
