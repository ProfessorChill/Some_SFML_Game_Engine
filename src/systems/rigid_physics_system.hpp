#ifndef SYSTEMS_RIGID_PHYSICS_SYSTEM
#define SYSTEMS_RIGID_PHYSICS_SYSTEM

#include <iostream>

#include "../ecs.hpp"

#include "../tmx-parser/map.hpp"

#include "../physics/aabb.hpp"

#include "../components/renderable.hpp"
#include "../components/rigidbody.hpp"
#include "../components/transform.hpp"

class Game;

class RigidPhysicsSystem : public ecs::System
{
public:
	void init(ecs::Coordinator *coordinator, Game *game)
	{
		mCoordinator = coordinator;
		mGame = game;
	}

	void update(tmx::Map *map)
	{
		// I have no clue what a better way to sort sets would be.
		std::set<ecs::Entity, EntityComparator> srtd(
		    {}, EntityComparator{.sCoordinator = mCoordinator});
		for (auto &entity : mEntities) {
			srtd.insert(entity);
		}

		for (auto const &entity : srtd) {
			auto &rigidbody = mCoordinator->getComponent<RigidBody>(entity);
			auto &transform = mCoordinator->getComponent<Transform>(entity);
			auto const &renderable = mCoordinator->getComponent<Renderable>(entity);

			physics::Object o1 =
			    physics::valuesToObject(&rigidbody, &transform, &renderable);

			// Iterate other entities.
			for (auto const &entityCol : srtd) {
				if (entity == entityCol) {
					continue;
				}

				auto const &rigidbodyCol =
				    mCoordinator->getComponent<RigidBody>(entityCol);
				auto const &transformCol =
				    mCoordinator->getComponent<Transform>(entityCol);
				auto const &renderableCol =
				    mCoordinator->getComponent<Renderable>(entityCol);

				physics::Object o2 = physics::valuesToObject(
				    &rigidbodyCol, &transformCol, &renderableCol);

				// Apply collision
				physics::Manifold manifold{
				    .A = &o1,
				    .B = &o2,
				    .penetration = 0.f,
				    .normal = sf::Vector2f(0.f, 0.f),
				};
				sf::Rect<float> intersection = physics::basicCollCheck(&manifold);

				if (intersection.height != 0 || intersection.top != 0 ||
				    intersection.left != 0 || intersection.width != 0) {
					transform.position.x -= rigidbody.velocity.x;
					rigidbody.velocity.x = 0;

					transform.position.y -= rigidbody.velocity.y;
					rigidbody.velocity.y = 0;
				}
			}

			// Iterate BLOCKING tiles
			for (auto const &layer : map->layers) {
				if (!layer.isBlocking) {
					continue;
				}

				float tileWidth = static_cast<float>(layer.tileset.tileWidth);
				float tileHeight = static_cast<float>(layer.tileset.tileHeight);

				for (auto const &tileGroup : layer.tiles) {
					for (auto const &tile : tileGroup) {
						if (!tile.isBlocking) {
							continue;
						}

						sf::Rect<float> tileCol(static_cast<float>(tile.x),
									static_cast<float>(tile.y),
									tileWidth, tileHeight);

						if (o1.rect.intersects(tileCol)) {
							transform.position.x -=
							    rigidbody.velocity.x;
							rigidbody.velocity.x = 0;

							transform.position.y -=
							    rigidbody.velocity.y;
							rigidbody.velocity.y = 0;
						}
					}
				}
			}
		}
	}

private:
	// This isn't the same as the render systems comparator, this one sorts x, subsorts y.
	struct EntityComparator {
		bool operator()(const ecs::Entity &entity1, const ecs::Entity &entity2) const
		{
			auto const &transform1 = sCoordinator->getComponent<Transform>(entity1);
			auto const &transform2 = sCoordinator->getComponent<Transform>(entity2);

			if (transform1.position.x == transform2.position.x) {
				return transform1.position.y < transform2.position.y;
			}

			return transform1.position.x < transform2.position.x;
		}

		ecs::Coordinator *sCoordinator;
	};

private:
	ecs::Coordinator *mCoordinator;
	Game *mGame;
};

#endif
