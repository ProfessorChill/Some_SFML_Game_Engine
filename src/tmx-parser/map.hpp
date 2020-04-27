#ifndef TMX_PARSER_MAP_HPP
#define TMX_PARSER_MAP_HPP

#include <SFML/Graphics.hpp>
#include <iostream>
#include <string>
#include <tinyxml2.h>
#include <vector>

#include "layer.hpp"
#include "object-group.hpp"
#include "tileset.hpp"

#include "../components/transform.hpp"

namespace tmx
{
class Map
{
public:
	Map()
	{
	}
	explicit Map(const std::string &basePath, const std::string &filename);

	double version;
	std::string orientation;
	std::string renderOrder;

	unsigned int width;
	unsigned int height;
	unsigned int tileWidth;
	unsigned int tileHeight;

	std::vector<Layer> layers;
	std::vector<Tileset> tilesets;
	std::vector<ObjectGroup> objectGroups;

	void draw(sf::RenderWindow &window, sf::Time deltaTime);
	void drawPosition(sf::RenderWindow &window, sf::Time deltaTime, const Transform &entityPos);
	void drawRegion(sf::RenderWindow &window, sf::Time deltaTime, sf::Rect<float> region);
	void update(sf::Time deltaTime);
};
} // namespace tmx

#endif
