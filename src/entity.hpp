#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "animated-sprite.hpp"

class Entity : sf::Transformable
{
    public:
	Entity() {}
	explicit Entity(sf::Texture *texture, unsigned int id);
	bool operator==(const Entity &rhs) { return this->id == rhs.id; }
	bool operator==(const Entity *rhs) { return this->id == rhs->id; }

	unsigned int id = 0;

	void draw(sf::RenderWindow &window, const sf::Time deltaTime);
	void update(const sf::Time deltaTime);
	void keyPress(sf::Keyboard::Key &key);
	void keyRelease(sf::Keyboard::Key &key);

	std::map<std::string, Animation> animations;

	// DEBUGGING
	bool displayCollision = true;

	// VERY temporary movement handler
	float acceleration = 0.1;
	float deceleration = 0.25;
	float maxWalkSpeed = 2.0;
	float maxRunSpeed = 4.0;

	bool movementUp;
	bool movementDown;
	bool movementLeft;
	bool movementRight;
	bool running;

	sf::Vector2f lastPos;
	sf::Vector2f pos;
	sf::Vector2f vel;

	sf::FloatRect collRect;
	AnimatedSprite sprite;

    private:
	sf::Texture *texture;

	Animation *currentAnimation;
};

#endif
