#ifndef TMX_PARSER_LAYER_HPP
#define TMX_PARSER_LAYER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <algorithm>
#include <cmath>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <tinyxml2.h>
#include <vector>

#include "tile.hpp"
#include "tileset.hpp"

#include "../components/transform.hpp"

namespace tmx
{
enum Encoding {
	XML,
	BASE64_UNCOMPRESSED,
	BASE64_GZIP_COMPRESSED,
	BASE64_ZLIB_COMPRESSED,
	CSV,
};

class Layer
{
public:
	Layer()
	{
	}
	explicit Layer(tinyxml2::XMLElement *layerElement);

	unsigned int id;
	std::string name;
	unsigned int width;
	unsigned int height;

	std::vector<int> data;
	std::vector<std::vector<MapTile>> tiles;

	Tileset tileset;

	tmx::Encoding encoding;
	sf::Sprite sprite;

	void init();
	void draw(sf::RenderWindow &window, sf::Time deltaTime);
	void drawPosition(sf::RenderWindow &window, sf::Time deltaTime, const Transform &entityPos);
	void drawRegion(sf::RenderWindow &window, sf::Time deltaTime, sf::Rect<float> region);
	void update(sf::Time deltaTime);

	bool isBlocking;
	bool isOverlay;

private:
	sf::IntRect entityPosToTextureRect(int x, int y)
	{
		if (x < 0 || y < 0) {
			return sf::IntRect();
		}

		return sf::IntRect(((this->data[x * this->width + y] - this->tileset.firstGid) %
				    this->tileset.columns) *
				       this->tileset.tileWidth,
				   ((this->data[x * this->width + y] - this->tileset.firstGid) /
				    this->tileset.columns) *
				       this->tileset.tileHeight,
				   this->tileset.tileWidth, this->tileset.tileHeight);
	}
};
} // namespace tmx

#endif
