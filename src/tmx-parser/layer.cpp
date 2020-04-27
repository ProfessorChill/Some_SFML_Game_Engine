#include "layer.hpp"

tmx::Layer::Layer(tinyxml2::XMLElement *layerElement)
{
	tinyxml2::XMLElement *data = layerElement->FirstChildElement("data");
	tinyxml2::XMLElement *properties = layerElement->FirstChildElement("properties");

	this->id = 0;
	this->width = 100;
	this->height = 100;

	this->name =
	    layerElement->Attribute("name") ? layerElement->Attribute("name") : "undefined";

	if (dbg::handleXMLError(layerElement->QueryUnsignedAttribute("id", &this->id))) {
		std::ostringstream errMsg;
		errMsg << "Attempting to continue without layer id, using default " << this->id
		       << ".";

		dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::WARNING);
	}

	if (dbg::handleXMLError(layerElement->QueryUnsignedAttribute("width", &this->width))) {
		std::ostringstream errMsg;
		errMsg << "Attempting to continue without layer width, using default "
		       << this->width << ".";

		dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::WARNING);
	}

	if (dbg::handleXMLError(layerElement->QueryUnsignedAttribute("height", &this->height))) {
		std::ostringstream errMsg;
		errMsg << "Attempting to continue without layer height, using default "
		       << this->height << ".";

		dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::WARNING);
	}

	if (!data || !data->GetText()) {
		std::ostringstream errMsg;
		errMsg << "TMX Parser layer \"" << this->name
		       << "\" has no data, stopping layer parsing.";

		dbg::printMessage(errMsg.str().c_str(), dbg::Urgency::WARNING);

		return;
	}

	std::string dataEncoding;
	std::string dataCompression;

	// If true, all objects on the layer will block physics objects.
	isBlocking = false;

	// If true, render over entities
	isOverlay = false;

	if (data->Attribute("compression") != nullptr) {
		dataCompression = data->Attribute("compression");
	}

	if (data->Attribute("encoding") != nullptr) {
		dataEncoding = data->Attribute("encoding");
	}

	if (dataEncoding == "csv") {
		this->encoding = tmx::Encoding::CSV;
	} else if (dataEncoding == "base64") {
		if (dataCompression == "gzip") {
			this->encoding = tmx::Encoding::BASE64_GZIP_COMPRESSED;
		} else if (dataCompression == "zlib") {
			this->encoding = tmx::Encoding::BASE64_ZLIB_COMPRESSED;
		} else {
			this->encoding = tmx::Encoding::BASE64_UNCOMPRESSED;
		}
	} else {
		this->encoding = tmx::Encoding::XML;
	}

	if (properties != nullptr) {
		tinyxml2::XMLElement *pListElement = properties->FirstChildElement("property");

		std::string propName =
		    pListElement->Attribute("name") ? pListElement->Attribute("name") : "";

		if (propName.compare("isBlocking") == 0) {
			pListElement->QueryBoolAttribute("value", &isBlocking);
		}

		if (propName.compare("isOverlay") == 0) {
			pListElement->QueryBoolAttribute("value", &isOverlay);
		}
	}

	switch (this->encoding) {
		case tmx::Encoding::CSV: {
			std::string dataText = data->GetText();
			std::replace(dataText.begin(), dataText.end(), ',', ' ');

			std::stringstream ss(dataText);
			std::copy(std::istream_iterator<int>(ss), std::istream_iterator<int>(),
				  std::back_inserter(this->data));

			break;
		}

		case tmx::Encoding::XML: {
			dbg::printMessage("XML parsing is not implemented yet!",
					  dbg::Urgency::ERROR);
			return;
		}

		case tmx::Encoding::BASE64_UNCOMPRESSED: {
			dbg::printMessage("Uncompressed Base64 parsing is not implemented yet!",
					  dbg::Urgency::ERROR);
			return;
		}

		case tmx::Encoding::BASE64_GZIP_COMPRESSED: {
			dbg::printMessage("GZIP Compressed Base64 parsing is not implemented yet!",
					  dbg::Urgency::ERROR);
			return;
		}

		case tmx::Encoding::BASE64_ZLIB_COMPRESSED: {
			dbg::printMessage("ZLIB Compressed Base64 parsing is not implemented yet!",
					  dbg::Urgency::ERROR);
			return;
		}

		default: {
			dbg::printMessage("Unsupported compression method, leaving layer.",
					  dbg::Urgency::ERROR);
			return;
		}
	}
}

/* This is called after the sprite gets set. */
void tmx::Layer::init()
{
	int iw = static_cast<int>(this->width); // Int Width
	int fGid = this->tileset.firstGid;
	int dataSize = this->data.size();
	sf::RectangleShape rect(sf::Vector2f(32.f, 32.f));

	int dataPos;

	// Assign the data to the maptiles vector
	for (unsigned int x = 0; x < this->width; x++) {
		std::vector<MapTile> maptile;

		for (unsigned int y = 0; y < this->height; y++) {
			dataPos = x * iw + y;

			if (dataSize <= dataPos || (this->data[dataPos] - fGid) < 0) {
				continue;
			}

			maptile.push_back(MapTile{
			    .x = y * this->tileset.tileWidth,
			    .y = x * this->tileset.tileHeight,
			    .isBlocking = isBlocking,
			});
		}

		tiles.push_back(maptile);
	}
}

void tmx::Layer::draw(sf::RenderWindow &window, sf::Time deltaTime)
{
	for (unsigned int x = 0; x < this->width; x++) {
		for (unsigned int y = 0; y < this->height; y++) {
			if ((this->data[x * this->width + y] - this->tileset.firstGid) < 0) {
				continue;
			}

			this->sprite.setTextureRect(entityPosToTextureRect(x, y));
			// clang-format off
			this->sprite.setPosition(
				(y * this->tileset.tileWidth),
				(x * this->tileset.tileHeight)
			);
			// clang-format on

			window.draw(this->sprite);
		}
	}
}

void tmx::Layer::drawPosition(sf::RenderWindow &window, sf::Time deltaTime,
			      const Transform &entityPos)
{
	if (!this->isOverlay) {
		return;
	}

	int entityXPos =
	    static_cast<int>(std::floor(entityPos.position.x) / this->tileset.tileWidth);
	int entityYPos =
	    static_cast<int>(std::floor(entityPos.position.y) / this->tileset.tileHeight);

	int iw = static_cast<int>(this->width);	 // Int Width
	int ih = static_cast<int>(this->height); // Int Height

	// Out of bounds check.
	if (entityXPos < 0 || entityXPos > iw || entityYPos < 0 || entityYPos > ih) {
		return;
	}

	// There is a better way to do this in a for loop, I'm just lazy so I'm doing it manually.
	// It's that contradictory?

	// Store firstGid in shorthand to prevent compression.
	int fGid = this->tileset.firstGid;
	int dataSize = this->data.size();
	sf::RectangleShape rect(sf::Vector2f(32.f, 32.f));

	// Same position as entity
	int dataPos = entityYPos * iw + entityXPos;
	if (dataSize >= dataPos && this->data[dataPos] - fGid >= 0) {
		this->sprite.setTextureRect(entityPosToTextureRect(entityYPos, entityXPos));
		this->sprite.setPosition(entityXPos * this->tileset.tileWidth,
					 entityYPos * this->tileset.tileHeight);

		window.draw(this->sprite);
	}

	// Below of entity
	dataPos = (entityYPos + 1) * iw + entityXPos;
	if (dataSize >= dataPos && this->data[dataPos] - fGid >= 0) {
		this->sprite.setTextureRect(entityPosToTextureRect(entityYPos + 1, entityXPos));
		this->sprite.setPosition(entityXPos * this->tileset.tileWidth,
					 (entityYPos + 1) * this->tileset.tileHeight);

		window.draw(this->sprite);
	}

	// Above the entity
	dataPos = (entityYPos - 1) * iw + entityXPos;
	if (dataSize >= dataPos && this->data[dataPos] - fGid >= 0) {
		this->sprite.setTextureRect(entityPosToTextureRect(entityYPos - 1, entityXPos));
		this->sprite.setPosition(entityXPos * this->tileset.tileWidth,
					 (entityYPos - 1) * this->tileset.tileHeight);

		window.draw(this->sprite);
	}

	// Right of entity
	dataPos = entityYPos * iw + (entityXPos + 1);
	if (dataSize >= dataPos && this->data[dataPos] - fGid >= 0) {
		this->sprite.setTextureRect(entityPosToTextureRect(entityYPos, entityXPos + 1));
		this->sprite.setPosition((entityXPos + 1) * this->tileset.tileWidth,
					 entityYPos * this->tileset.tileHeight);

		window.draw(this->sprite);
	}

	// Left of entity
	dataPos = entityYPos * iw + (entityXPos - 1);
	if (dataSize >= dataPos && this->data[dataPos] - fGid >= 0) {
		this->sprite.setTextureRect(entityPosToTextureRect(entityYPos, entityXPos - 1));
		this->sprite.setPosition((entityXPos - 1) * this->tileset.tileWidth,
					 entityYPos * this->tileset.tileHeight);

		window.draw(this->sprite);
	}

	// Top left of entity
	dataPos = (entityYPos - 1) * iw + (entityXPos - 1);
	if (dataSize >= dataPos && this->data[dataPos] - fGid >= 0) {
		this->sprite.setTextureRect(entityPosToTextureRect(entityYPos - 1, entityXPos - 1));
		this->sprite.setPosition((entityXPos - 1) * this->tileset.tileWidth,
					 (entityYPos - 1) * this->tileset.tileHeight);

		window.draw(this->sprite);
	}

	// Top right of entity
	dataPos = (entityYPos - 1) * iw + (entityXPos + 1);
	if (dataSize >= dataPos && this->data[dataPos] - fGid >= 0) {
		this->sprite.setTextureRect(entityPosToTextureRect(entityYPos - 1, entityXPos + 1));
		this->sprite.setPosition((entityXPos + 1) * this->tileset.tileWidth,
					 (entityYPos - 1) * this->tileset.tileHeight);

		window.draw(this->sprite);
	}

	// Bottom left of entity
	dataPos = (entityYPos + 1) * iw + (entityXPos - 1);
	if (dataSize >= dataPos && this->data[dataPos] - fGid >= 0) {
		this->sprite.setTextureRect(entityPosToTextureRect(entityYPos + 1, entityXPos - 1));
		this->sprite.setPosition((entityXPos - 1) * this->tileset.tileWidth,
					 (entityYPos + 1) * this->tileset.tileHeight);

		window.draw(this->sprite);
	}

	// Bottom right of entity
	dataPos = (entityYPos + 1) * iw + (entityXPos + 1);
	if (dataSize >= (dataPos) && this->data[dataPos] - fGid >= 0) {
		this->sprite.setTextureRect(entityPosToTextureRect(entityYPos + 1, entityXPos + 1));
		this->sprite.setPosition((entityXPos + 1) * this->tileset.tileWidth,
					 (entityYPos + 1) * this->tileset.tileHeight);

		window.draw(this->sprite);
	}
}

void tmx::Layer::drawRegion(sf::RenderWindow &window, sf::Time deltaTime, sf::Rect<float> region)
{
	// Clamp the regions x and y values to int.
	int xMin = static_cast<int>(std::floor(region.left / this->tileset.tileWidth));
	int yMin = static_cast<int>(std::floor(region.top / this->tileset.tileHeight));
	int xMax =
	    static_cast<int>(std::ceil((region.left + region.width) / this->tileset.tileWidth));
	int yMax =
	    static_cast<int>(std::ceil((region.top + region.height) / this->tileset.tileHeight));

	int dataSize = this->data.size();
	int fGid = this->tileset.firstGid;
	int iw = static_cast<int>(this->width); // Int Width

	// I know this is backwards, I initially did the math backwards and never fixed it.
	for (int x = yMin; x < yMax; x++) {
		for (int y = xMin; y < xMax; y++) {
			int dataPos = x * iw + y;

			if (dataSize <= dataPos || (this->data[dataPos] - fGid) < 0) {
				continue;
			}

			this->sprite.setTextureRect(entityPosToTextureRect(x, y));
			// clang-format off
			this->sprite.setPosition(
				(y * this->tileset.tileWidth),
				(x * this->tileset.tileHeight)
			);
			// clang-format on

			window.draw(this->sprite);
		}
	}
}
