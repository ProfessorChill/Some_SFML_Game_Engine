#ifndef ENTITY_HANDLER_HPP
#define ENTITY_HANDLER_HPP

#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <algorithm>
#include <iostream>
#include <vector>

#include "entity.hpp"

class EntityHandler
{
    public:
	EntityHandler() {}

	void draw(sf::RenderWindow &window, const sf::Time deltaTime);
	void update(const sf::Time deltaTime);

	Entity *addEntity(sf::Texture *texture);
	void removeEntity(Entity *entity);
	void removeEntity(unsigned int entity);
	Entity *getEntityById(unsigned int entity);
	// Entity *getEntityByTag(const std::string &entity);

	size_t getEntityCount();

    private:
	unsigned int nextId = 0;
	std::vector<Entity> entities;
};

#endif
