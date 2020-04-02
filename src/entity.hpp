#ifndef ENTITY_HPP
#define ENTITY_HPP

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <algorithm>
#include <iostream>
#include <map>
#include <math.h>
#include <string>
#include <vector>

#include "animated-sprite.hpp"
#include "movement.hpp"

enum Direction {
	UP,
	RIGHT,
	DOWN,
	LEFT,
};

class Entity : sf::Transformable, Movement
{
    public:
	Entity() {}
	explicit Entity(sf::Texture *texture, unsigned int id);
	bool operator==(const Entity &rhs) { return this->id == rhs.id; }
	bool operator==(const Entity *rhs) { return this->id == rhs->id; }
	bool operator<(const Entity &rhs) const { return this->id < rhs.id; }

	unsigned int id = 0;

	virtual void draw(sf::RenderWindow &window, const sf::Time deltaTime);
	virtual void update(const sf::Time deltaTime);
	virtual void keyPress(sf::Keyboard::Key &key);
	virtual void keyRelease(sf::Keyboard::Key &key);

	virtual void checkCollision(Entity *other);

	std::map<std::string, Animation> animations;

	// DEBUGGING
	bool displayCollision = true;

	sf::Vector2f lastPos;
	sf::Vector2f pos;
	sf::Vector2f vel;

	sf::FloatRect collRect;
	AnimatedSprite sprite;

	std::shared_ptr<b2Body> body;

    private:
	sf::Texture *texture;

	Animation *currentAnimation;
};

#endif
