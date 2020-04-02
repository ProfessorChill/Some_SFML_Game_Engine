#include "entity-handler.hpp"

struct EntityComparator {
	bool operator()(Entity *entity1, Entity *entity2)
	{
		if (entity1->pos.y == entity2->pos.y) {
			return entity1 < entity2;
		}

		return entity1->pos.y < entity2->pos.y;
	}
};

void EntityHandler::draw(sf::RenderWindow &window, const sf::Time deltaTime,
			 sf::FloatRect region)
{
	// Calls draw for all entities in the entity list
	this->visibleEntities = getEntitiesInRegion(region);
	std::sort(visibleEntities.begin(), visibleEntities.end(),
		  EntityComparator());

	for (Entity *entity : visibleEntities) {
		entity->draw(window, deltaTime);
	}
}

void EntityHandler::update(const sf::Time deltaTime)
{
	// Calls update for all entities in the entity list

	for (Entity *entity : this->visibleEntities) {
		for (Entity *entityCheck : this->visibleEntities) {
			if (entity == entityCheck) {
				continue;
			}

			entity->checkCollision(entityCheck);
		}

		entity->update(deltaTime);
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

Entity *EntityHandler::getEntityById(unsigned int entityId)
{
	for (Entity &entity : this->entities) {
		if (entity.id == entityId) {
			return &entity;
		}
	}

	return nullptr;
}

std::vector<Entity *> EntityHandler::getEntitiesInRegion(sf::FloatRect region)
{
	std::vector<Entity *> entityMatches;

	for (Entity &entity : this->entities) {
		if (region.contains(entity.pos.x, entity.pos.y)) {
			entityMatches.push_back(&entity);
		}
	}

	return entityMatches;
}

size_t EntityHandler::getEntityCount()
{
	// Get the amount of entities

	return this->entities.size();
}
