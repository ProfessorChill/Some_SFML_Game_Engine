#ifndef TMX_PARSER_LAYER_HPP
#define TMX_PARSER_LAYER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <algorithm>
#include <iostream>
#include <iterator>
#include <sstream>
#include <string>
#include <tinyxml2.h>
#include <vector>

#include "tile.hpp"
#include "tileset.hpp"

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
	Layer() {}
	explicit Layer(tinyxml2::XMLElement *layerElement);

	unsigned int id;
	std::string name;
	unsigned int width;
	unsigned int height;

	std::vector<unsigned int> data;
	std::vector<std::vector<MapTile>> tiles;

	Tileset tileset;

	tmx::Encoding encoding;
	sf::Sprite sprite;

	void draw(sf::RenderWindow &window, sf::Time deltaTime);
	void update(sf::Time deltaTime);
};
} // namespace tmx

#endif
