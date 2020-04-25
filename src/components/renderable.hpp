#ifndef COMPONENTS_RENDERABLE_HPP
#define COMPONENTS_RENDERABLE_HPP

#include <SFML/Graphics/Color.hpp>
#include <SFML/System/Vector2.hpp>

struct Renderable {
	sf::Color color;
	sf::Vector2f size;
};

#endif
