#ifndef ENTITY_HANDLER_HPP
#define ENTITY_HANDLER_HPP

#include <Box2D/Box2D.h>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <algorithm>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>

#include "entity.hpp"

class EntityHandler
{
    public:
	EntityHandler() {}

	void draw(sf::RenderWindow &window, const sf::Time deltaTime,
		  sf::FloatRect region);
	void update(const sf::Time deltaTime);

	// Add, removal, getting
	Entity *addEntity(sf::Texture *texture);
	void removeEntity(Entity *entity);
	void removeEntity(unsigned int entity);
	Entity *getEntityById(unsigned int entity);
	// std::vector<Entity *> getEntitiesByTag(const std::string &entity);

	// Gets all entities in a region, this should be used to render entities
	// in a section.
	std::vector<Entity *> getEntitiesInRegion(sf::FloatRect region);

	size_t getEntityCount();

    private:
	unsigned int nextId = 0;
	std::vector<Entity> entities;
	std::vector<Entity *> visibleEntities;
};

#endif
