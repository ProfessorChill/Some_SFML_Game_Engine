#ifndef SYSTEMS_RENDER_SYSTEM_HPP
#define SYSTEMS_RENDER_SYSTEM_HPP

#include <SFML/Graphics.hpp>

#include "../ecs.hpp"
#include "../game.hpp"

#include "../tmx-parser/map.hpp"

#include "../components/renderable.hpp"
#include "../components/transform.hpp"

class RenderSystem : public ecs::System
{
public:
	void init(ecs::Coordinator *coordinator, Game *game)
	{
		mCoordinator = coordinator;
		mGame = game;
	}

	void draw(tmx::Map *map, sf::Time time, sf::Rect<float> region)
	{
		// I have no clue what a better way to sort sets would be.
		std::set<ecs::Entity, EntityComparator> srtd(
		    {}, EntityComparator{.sCoordinator = mCoordinator});
		for (auto &entity : mEntities) {
			srtd.insert(entity);
		}

		map->drawRegion(mGame->window, time, region);

		// Draw the sorted set.
		for (auto const &entity : srtd) {
			auto const &transform = mCoordinator->getComponent<Transform>(entity);
			auto const &renderable = mCoordinator->getComponent<Renderable>(entity);

			// Temporaraly draw a rectangle for the entities.
			sf::RectangleShape rect(renderable.size);
			rect.setPosition(transform.position);
			rect.setFillColor(renderable.color);
			mGame->window.draw(rect);

			map->drawPosition(mGame->window, time, transform);
		}
	}

private:
	struct EntityComparator {
		bool operator()(const ecs::Entity &entity1, const ecs::Entity &entity2) const
		{
			auto const &transform1 = sCoordinator->getComponent<Transform>(entity1);
			auto const &transform2 = sCoordinator->getComponent<Transform>(entity2);

			if (transform1.position.y == transform2.position.y) {
				return transform1.position.x < transform2.position.x;
			}

			return transform1.position.y < transform2.position.y;
		}

		ecs::Coordinator *sCoordinator;
	};

private:
	ecs::Coordinator *mCoordinator;
	Game *mGame;
};

#endif
