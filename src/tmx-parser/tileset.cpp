#include "tileset.hpp"

tmx::Tileset::Tileset(const std::string &basePath, const std::string &filename, int firstGid)
    : firstGid(firstGid), source(filename)
{
	std::string newPath = basePath + "/resources/maps/" + filename;
	std::cout << newPath << std::endl;
	std::cout << "Loaded Tileset\n";

	tinyxml2::XMLDocument doc;
	doc.LoadFile(newPath.c_str());

	tinyxml2::XMLElement *mapRoot = doc.RootElement();

	this->version = std::stod(mapRoot->Attribute("version"));
	this->name = mapRoot->Attribute("name");
	this->tileWidth = std::stoi(mapRoot->Attribute("tilewidth"));
	this->tileHeight = std::stoi(mapRoot->Attribute("tileheight"));
	this->tileCount = std::stoi(mapRoot->Attribute("tilecount"));
	this->columns = std::stoi(mapRoot->Attribute("columns"));

	tinyxml2::XMLElement *imageElem = mapRoot->FirstChildElement("image");

	std::string imgSource = imageElem->Attribute("source");
	newPath = basePath + "/resources/tilesets/" + imgSource;

	std::cout << newPath << std::endl;

	this->texture.loadFromFile(newPath);
}
