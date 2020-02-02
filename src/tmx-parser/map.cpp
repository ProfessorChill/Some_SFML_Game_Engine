#include "map.hpp"

tmx::Map::Map(const std::string &basePath, const std::string &filename)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile((basePath + "/" + filename).c_str());

	tinyxml2::XMLElement *mapRoot = doc.RootElement();

	this->version = std::stod(mapRoot->Attribute("version"));
	this->orientation = mapRoot->Attribute("orientation");
	this->renderOrder = mapRoot->Attribute("renderorder");
	this->width = std::stoi(mapRoot->Attribute("width"));
	this->height = std::stoi(mapRoot->Attribute("height"));
	this->tileWidth = std::stoi(mapRoot->Attribute("tilewidth"));
	this->tileHeight = std::stoi(mapRoot->Attribute("tileheight"));

	tinyxml2::XMLElement *layerElement =
	    mapRoot->FirstChildElement("layer");

	tinyxml2::XMLElement *tilesetElement =
	    mapRoot->FirstChildElement("tileset");

	while (tilesetElement != nullptr) {
		std::string source = tilesetElement->Attribute("source");
		unsigned int firstGid =
		    std::stoi(tilesetElement->Attribute("firstgid"));

		this->tilesets.push_back(
		    tmx::Tileset(basePath, source, firstGid));

		tilesetElement = tilesetElement->NextSiblingElement("tileset");
	}

	while (layerElement != nullptr) {
		this->layers.push_back(tmx::Layer(layerElement));

		layerElement = layerElement->NextSiblingElement("layer");
	}

	for (size_t x = 0; x < this->layers.size(); ++x) {
		this->layers[x].sprite = sf::Sprite(this->tilesets[0].texture);
		this->layers[x].tileset = this->tilesets[0];
	}
}

void tmx::Map::draw(sf::RenderWindow &window, sf::Time deltaTime)
{
	for (size_t x = 0; x < this->layers.size(); ++x) {
		this->layers[x].draw(window, deltaTime);
	}
}
