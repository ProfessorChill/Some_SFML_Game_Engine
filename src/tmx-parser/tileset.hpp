#ifndef TMX_PARSER_TILESET_HPP
#define TMX_PARSER_TILESET_HPP

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <iostream>
#include <string>
#include <tinyxml2.h>
#include <vector>

namespace tmx
{
class Tileset
{
public:
	Tileset()
	{
	}
	explicit Tileset(const std::string &basePath, const std::string &filename, int firstGid);

	int firstGid;
	std::string source;

	double version;
	std::string name;
	unsigned int tileWidth;
	unsigned int tileHeight;
	unsigned int tileCount;
	unsigned int columns;

	sf::Texture texture;
};
} // namespace tmx

#endif
