#include "entity-handler.hpp"

void EntityHandler::draw(sf::RenderWindow &window, const sf::Time deltaTime)
{
	// Calls draw for all entities in the entity list

	for (Entity &entity : this->entities) {
		entity.draw(window, deltaTime);
	}
}

void EntityHandler::update(const sf::Time deltaTime)
{
	// Calls update for all entities in the entity list

	for (Entity &entity : this->entities) {
		entity.update(deltaTime);
	}
}

Entity *EntityHandler::addEntity(sf::Texture *texture)
{
	// Add an entity to the entity list

	this->entities.push_back(Entity(texture, this->nextId));
	this->nextId++;

	return &this->entities[this->entities.size() - 1];
}

void EntityHandler::removeEntity(Entity *entity)
{
	// Remove and entity from the entity list

	this->entities.erase(
	    std::remove(this->entities.begin(), this->entities.end(), entity),
	    this->entities.end());
}

void EntityHandler::removeEntity(unsigned int entity)
{
	// Remove an entity at x position

	this->entities.erase(this->entities.begin() + entity);
}

Entity *EntityHandler::getEntityById(unsigned int entity)
{
	return &this->entities[entity];
}

size_t EntityHandler::getEntityCount()
{
	// Get the amount of entities

	return this->entities.size();
}
