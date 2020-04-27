#include "map.hpp"
#include "../debug.hpp"

tmx::Map::Map(const std::string &basePath, const std::string &filename)
{
	tinyxml2::XMLDocument doc;
	doc.LoadFile((basePath + "/" + filename).c_str());

	// Set default values (in case of error).
	this->version = 0.0;
	this->orientation = "orthogonal";
	this->renderOrder = "right-down";
	this->width = 100;
	this->height = 100;
	this->tileWidth = 32;
	this->tileHeight = 32;

	if (doc.Error()) {
		std::ostringstream errMsg;
		errMsg << "Error parsing XML file " << (basePath + "/" + filename).c_str();

		dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::ERROR);
	}

	tinyxml2::XMLElement *mapRoot = doc.RootElement();

	if (dbg::handleXMLError(mapRoot->QueryDoubleAttribute("version", &this->version))) {
		std::ostringstream errMsg;
		errMsg << "Attempting to continue without map version, using default "
		       << this->version << ".";

		dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::WARNING);
	}

	if (mapRoot->Attribute("orientation")) {
		this->orientation = mapRoot->Attribute("orientation");
	} else {
		std::ostringstream errMsg;
		errMsg << "Attempting to continue without map orientation, using default "
		       << this->orientation << ".";

		dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::WARNING);
	}

	if (mapRoot->Attribute("renderorder")) {
		this->renderOrder = mapRoot->Attribute("renderorder");
	} else {
		std::ostringstream errMsg;
		errMsg << "Attempting to continue without map render order, using default "
		       << this->renderOrder << ".";

		dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::WARNING);
	}

	if (dbg::handleXMLError(mapRoot->QueryUnsignedAttribute("width", &this->width))) {
		std::ostringstream errMsg;
		errMsg << "Attempting to continue without map width, using default " << this->width
		       << ".";

		dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::WARNING);
	}

	if (dbg::handleXMLError(mapRoot->QueryUnsignedAttribute("height", &this->height))) {
		std::ostringstream errMsg;
		errMsg << "Attempting to continue without map height, using default "
		       << this->height << ".";

		dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::WARNING);
	}

	if (dbg::handleXMLError(mapRoot->QueryUnsignedAttribute("tilewidth", &this->tileWidth))) {
		std::ostringstream errMsg;
		errMsg << "Attempting to continue without map tile width, using default "
		       << this->tileWidth << ".";

		dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::WARNING);
	}

	if (dbg::handleXMLError(mapRoot->QueryUnsignedAttribute("tileheight", &this->tileHeight))) {
		std::ostringstream errMsg;
		errMsg << "Attempting to continue without map tile height, using default "
		       << this->tileHeight << ".";

		dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::WARNING);
	}

	tinyxml2::XMLElement *tilesetElement = mapRoot->FirstChildElement("tileset");
	tinyxml2::XMLElement *layerElement = mapRoot->FirstChildElement("layer");
	tinyxml2::XMLElement *objectGroupElement = mapRoot->FirstChildElement("objectgroup");

	while (tilesetElement != nullptr) {
		std::string source = tilesetElement->Attribute("source");

		unsigned int firstGid = 0;
		if (dbg::handleXMLError(
			tilesetElement->QueryUnsignedAttribute("firstgid", &firstGid))) {
			std::ostringstream errMsg;
			errMsg << "Attempting to continue without layer GID, using default "
			       << firstGid << ".";

			dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::WARNING);
		}

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

	for (Layer &layer : this->layers) {
		layer.sprite = sf::Sprite(this->tilesets[0].texture);
		layer.tileset = this->tilesets[0];
		layer.init(); // Called after tiles and sprites are set.
	}
}

void tmx::Map::draw(sf::RenderWindow &window, sf::Time deltaTime)
{
	for (Layer &layer : this->layers) {
		layer.draw(window, deltaTime);
	}
}

void tmx::Map::drawPosition(sf::RenderWindow &window, sf::Time deltaTime,
			    const Transform &entityPos)
{
	for (Layer &layer : this->layers) {
		layer.drawPosition(window, deltaTime, entityPos);
	}
}

void tmx::Map::drawRegion(sf::RenderWindow &window, sf::Time deltaTime, sf::Rect<float> region)
{
	for (Layer &layer : this->layers) {
		layer.drawRegion(window, deltaTime, region);
	}
}
