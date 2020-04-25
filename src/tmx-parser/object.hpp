#ifndef TMX_PARSER_OBJECT_HPP
#define TMX_PARSER_OBJECT_HPP

#include <iostream>
#include <string>
#include <tinyxml2.h>

namespace tmx
{
enum ObjectProperty {
	SpawnPoint,
	NPCSpawnPoint,
	Event,
	None,
};

class Object
{
public:
	Object(tinyxml2::XMLElement *objectElement)
	{
		objectElement->QueryUnsignedAttribute("id", &this->id);
		this->name = objectElement->Attribute("name");
		this->type = objectElement->Attribute("type");
		objectElement->QueryUnsignedAttribute("x", &this->x);
		objectElement->QueryUnsignedAttribute("y", &this->y);
		objectElement->QueryUnsignedAttribute("width", &this->width);
		objectElement->QueryUnsignedAttribute("height", &this->height);
		objectElement->QueryBoolAttribute("visible", &this->visible);

		if (this->visible) {
			std::cout << "Visible\n";
		}

		tinyxml2::XMLElement *properties = objectElement->FirstChildElement("properties");

		if (this->type.compare("event") == 0) {
			if (properties != nullptr) {
				tinyxml2::XMLElement *pListElement =
				    properties->FirstChildElement("property");

				this->property = tmx::ObjectProperty::Event;

				while (pListElement != nullptr) {
					std::string attrName = pListElement->Attribute("name");

					if (attrName.compare("script") == 0) {
						luaScript = pListElement->Attribute("value");
					}

					pListElement = pListElement->NextSiblingElement("property");
				}
			}
		} else if (this->type.compare("spawn") == 0) {
			this->property = tmx::ObjectProperty::SpawnPoint;
		} else if (this->type.compare("npc_spawn") == 0) {
			this->property = tmx::ObjectProperty::NPCSpawnPoint;
		} else {
			this->property = tmx::ObjectProperty::None;
		}
	}

	unsigned int id;
	std::string name;
	std::string type;
	unsigned int x;
	unsigned int y;
	unsigned int width;
	unsigned int height;
	bool visible = true;

	std::string luaScript;

	ObjectProperty property;
};
} // namespace tmx

#endif
