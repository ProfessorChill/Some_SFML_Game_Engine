#include "layer.hpp"

tmx::Layer::Layer(tinyxml2::XMLElement *layerElement)
{
	tinyxml2::XMLElement *data = layerElement->FirstChildElement("data");
	tinyxml2::XMLElement *properties = layerElement->FirstChildElement("properties");

	layerElement->QueryUnsignedAttribute("id", &this->id);
	this->name = layerElement->Attribute("name");
	layerElement->QueryUnsignedAttribute("width", &this->width);
	layerElement->QueryUnsignedAttribute("height", &this->height);

	std::string dataEncoding;
	std::string dataCompression;

	// If true, all objects on the layer will block physics objects.
	isBlocking = false;

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

		std::string propName = pListElement->Attribute("name");

		if (propName.compare("isBlocking") == 0) {
			std::string propValue = pListElement->Attribute("value");
			if (propValue.compare("true") == 0) {
				isBlocking = true;
			}
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
			std::cout << "[\x1b[31mERROR\x1b[0m] XML parsing is not "
				     "implemented yet!\n";

			break;
		}

		case tmx::Encoding::BASE64_UNCOMPRESSED: {
			std::cout << "[\x1b[31mERROR\x1b[0m] Uncompressed Base64 "
				     "parsing is not implemented yet!\n";

			break;
		}

		case tmx::Encoding::BASE64_GZIP_COMPRESSED: {
			std::cout << "[\x1b[31mERROR\x1b[0m] GZIP Compressed Base64 "
				     "parsing is not implemented yet!\n";

			break;
		}

		case tmx::Encoding::BASE64_ZLIB_COMPRESSED: {
			std::cout << "[\x1b[31mERROR\x1b[0m] ZLIB Compressed Base64 "
				     "parsing is not implemented yet!\n";

			break;
		}

		default:
			break;
	}
}

/* This is called after the sprite gets set. */
void tmx::Layer::init()
{
	// Assign the data to the maptiles vector
	unsigned int xPos = 0;
	unsigned int yPos = 0;

	for (unsigned int x = 0; x < this->width; x++) {
		std::vector<MapTile> maptile;

		for (unsigned int y = 0; y < this->height; y++) {
			if (xPos >= this->width * this->tileset.tileWidth) {
				yPos += this->tileset.tileHeight;
				xPos = 0;
			}

			if ((this->data[x * this->width + y] - this->tileset.firstGid) < 0) {
				xPos += this->tileset.tileWidth;

				continue;
			}

			maptile.push_back(MapTile{
			    .x = xPos,
			    .y = yPos,
			    .isBlocking = isBlocking,
			});

			xPos += this->tileset.tileWidth;
		}

		tiles.push_back(maptile);
	}
}

void tmx::Layer::draw(sf::RenderWindow &window, sf::Time deltaTime)
{
	unsigned int xPos = 0;
	unsigned int yPos = 0;

	for (unsigned int x = 0; x < this->width; x++) {
		for (unsigned int y = 0; y < this->height; y++) {
			if (xPos >= this->width * this->tileset.tileWidth) {
				yPos += this->tileset.tileHeight;
				xPos = 0;
			}

			if ((this->data[x * this->width + y] - this->tileset.firstGid) < 0) {
				xPos += this->tileset.tileWidth;

				continue;
			}

			this->sprite.setTextureRect(sf::IntRect(
			    ((this->data[x * this->width + y] - this->tileset.firstGid) %
			     this->tileset.columns) *
				this->tileset.tileWidth,
			    ((this->data[x * this->width + y] - this->tileset.firstGid) /
			     this->tileset.columns) *
				this->tileset.tileHeight,
			    this->tileset.tileWidth, this->tileset.tileHeight));
			this->sprite.setPosition(sf::Vector2f(xPos, yPos));

			xPos += this->tileset.tileWidth;

			window.draw(this->sprite);
		}
	}
}
