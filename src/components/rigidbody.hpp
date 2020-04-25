#ifndef COMPONENTS_RIGIDBODY_HPP
#define COMPONENTS_RIGIDBODY_HPP

#include <SFML/System/Vector2.hpp>

struct RigidBody {
	sf::Vector2f velocity;
	sf::Vector2f acceleration;
	sf::Vector2f deceleration;
};

#endif
