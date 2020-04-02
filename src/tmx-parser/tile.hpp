#include <SFML/Graphics.hpp>

enum MapTileType {
	mtAir,
	mtWall,
	mtSolid,
};

struct MapTile {
	MapTileType type;
	sf::Color light;
	sf::Vector2i index;
	char intensity;
	char absorb;
};
