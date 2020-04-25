#include "map.hpp"

tmx::Map::Map(const std::string &basePath, const std::string &filename)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile((basePath + "/" + filename).c_str());

	tinyxml2::XMLElement *mapRoot = doc.RootElement();

	mapRoot->QueryDoubleAttribute("version", &this->version);
	this->orientation = mapRoot->Attribute("orientation");
	this->renderOrder = mapRoot->Attribute("renderorder");
	mapRoot->QueryUnsignedAttribute("width", &this->width);
	mapRoot->QueryUnsignedAttribute("height", &this->height);
	mapRoot->QueryUnsignedAttribute("tilewidth", &this->tileWidth);
	mapRoot->QueryUnsignedAttribute("tileheight", &this->tileHeight);

	tinyxml2::XMLElement *tilesetElement = mapRoot->FirstChildElement("tileset");
	tinyxml2::XMLElement *layerElement = mapRoot->FirstChildElement("layer");
	tinyxml2::XMLElement *objectGroupElement = mapRoot->FirstChildElement("objectgroup");

	while (tilesetElement != nullptr) {
		std::string source = tilesetElement->Attribute("source");

		unsigned int firstGid = 0;
		tilesetElement->QueryUnsignedAttribute("firstgid", &firstGid);

		this->tilesets.push_back(tmx::Tileset(basePath, source, firstGid));

		tilesetElement = tilesetElement->NextSiblingElement("tileset");
	}

	while (layerElement != nullptr) {
		this->layers.push_back(tmx::Layer(layerElement));

		layerElement = layerElement->NextSiblingElement("layer");
	}

	while (objectGroupElement != nullptr) {
		this->objectGroups.push_back(tmx::ObjectGroup(objectGroupElement));

		objectGroupElement = objectGroupElement->NextSiblingElement("objectgroup");
	}

	for (size_t x = 0; x < this->layers.size(); ++x) {
		this->layers[x].sprite = sf::Sprite(this->tilesets[0].texture);
		this->layers[x].tileset = this->tilesets[0];
		this->layers[x].init(); // Called after tiles and sprites are set.
	}
}

void tmx::Map::draw(sf::RenderWindow &window, sf::Time deltaTime)
{
	for (size_t x = 0; x < this->layers.size(); ++x) {
		this->layers[x].draw(window, deltaTime);
	}
}
