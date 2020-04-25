#ifndef SYSTEMS_PLAYER_SYSTEM_HPP
#define SYSTEMS_PLAYER_SYSTEM_HPP

#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "../ecs.hpp"

#include "../tmx-parser/object-group.hpp"
#include "../tmx-parser/object.hpp"

#include "../components/movement.hpp"
#include "../components/player.hpp"
#include "../components/renderable.hpp"
#include "../components/rigidbody.hpp"
#include "../components/transform.hpp"

class Game;

class PlayerSystem : public ecs::System
{
public:
	void init(ecs::Coordinator *coordinator, Game *game)
	{
		mCoordinator = coordinator;
		mGame = game;
	}

	void initSpawns()
	{
		for (auto const &entity : mEntities) {
			auto &transform = mCoordinator->getComponent<Transform>(entity);
			auto &movement = mCoordinator->getComponent<MovementNew>(entity);
			auto &renderable = mCoordinator->getComponent<Renderable>(entity);

			for (tmx::ObjectGroup &objGroup : movement.mapPtr->objectGroups) {
				for (tmx::Object &obj : objGroup.objects) {
					switch (obj.property) {
						case tmx::ObjectProperty::SpawnPoint:
							transform.position.x =
							    (obj.x + (obj.width / 2)) -
							    (renderable.size.x / 2);
							transform.position.y =
							    (obj.y + (obj.height / 2)) -
							    (renderable.size.y / 2);
							break;

						default:
							break;
					}
				}
			}
		}
	}

	void update(sf::View *gameView)
	{
		for (auto const &entity : mEntities) {
			auto &transform = mCoordinator->getComponent<Transform>(entity);
			auto &rigidBody = mCoordinator->getComponent<RigidBody>(entity);
			auto &movement = mCoordinator->getComponent<MovementNew>(entity);
			auto &player = mCoordinator->getComponent<Player>(entity);
			auto &renderable = mCoordinator->getComponent<Renderable>(entity);

			if (movement.running) {
				if (movement.right && rigidBody.velocity.x < player.maxRunSpeed) {
					rigidBody.velocity.x += rigidBody.acceleration.x;
				}

				if (movement.left && rigidBody.velocity.x > -player.maxRunSpeed) {
					rigidBody.velocity.x -= rigidBody.acceleration.x;
				}

				if (movement.up && rigidBody.velocity.y > -player.maxRunSpeed) {
					rigidBody.velocity.y -= rigidBody.acceleration.y;
				}

				if (movement.down && rigidBody.velocity.y < player.maxRunSpeed) {
					rigidBody.velocity.y += rigidBody.acceleration.y;
				}
			} else {
				if (movement.right && rigidBody.velocity.x < player.maxWalkSpeed) {
					rigidBody.velocity.x += rigidBody.acceleration.x;
				}

				if (movement.left && rigidBody.velocity.x > -player.maxWalkSpeed) {
					rigidBody.velocity.x -= rigidBody.acceleration.x;
				}

				if (movement.up && rigidBody.velocity.y > -player.maxWalkSpeed) {
					rigidBody.velocity.y -= rigidBody.acceleration.y;
				}

				if (movement.down && rigidBody.velocity.y < player.maxWalkSpeed) {
					rigidBody.velocity.y += rigidBody.acceleration.y;
				}

				// Clamp the run speed.
				if (rigidBody.velocity.x > 0) {
					rigidBody.velocity.x =
					    std::min(player.maxWalkSpeed, rigidBody.velocity.x);
				}

				if (rigidBody.velocity.y > 0) {
					rigidBody.velocity.y =
					    std::min(player.maxWalkSpeed, rigidBody.velocity.y);
				}

				if (rigidBody.velocity.x < 0) {
					rigidBody.velocity.x =
					    std::max(-player.maxWalkSpeed, rigidBody.velocity.x);
				}

				if (rigidBody.velocity.y < 0) {
					rigidBody.velocity.y =
					    std::max(-player.maxWalkSpeed, rigidBody.velocity.y);
				}
			}

			// Apply deceleration on X
			if (rigidBody.velocity.x > 0 && !movement.right) {
				if (rigidBody.velocity.x < rigidBody.deceleration.x) {
					rigidBody.velocity.x = 0;
				} else {
					rigidBody.velocity.x -= rigidBody.deceleration.x;
				}
			} else if (rigidBody.velocity.x < 0 && !movement.left) {
				if (rigidBody.velocity.x > rigidBody.deceleration.x) {
					rigidBody.velocity.x = 0;
				} else {
					rigidBody.velocity.x += rigidBody.deceleration.x;
				}
			}
			transform.position.x += rigidBody.velocity.x;

			// Apply acceleration on Y
			if (rigidBody.velocity.y > 0 && !movement.down) {
				if (rigidBody.velocity.y < rigidBody.deceleration.y) {
					rigidBody.velocity.y = 0;
				} else {
					rigidBody.velocity.y -= rigidBody.deceleration.y;
				}
			} else if (rigidBody.velocity.y < 0 && !movement.up) {
				if (rigidBody.velocity.y > rigidBody.deceleration.y) {
					rigidBody.velocity.y = 0;
				} else {
					rigidBody.velocity.y += rigidBody.deceleration.y;
				}
			}
			transform.position.y += rigidBody.velocity.y;

			gameView->setCenter(getCenter(renderable, transform));
		}
	}

	void handleKeyDown(sf::Keyboard::Key key)
	{
		for (auto const &entity : mEntities) {
			auto &movement = mCoordinator->getComponent<MovementNew>(entity);

			switch (key) {
				case sf::Keyboard::Right:
				case sf::Keyboard::D:
					movement.right = true;
					break;

				case sf::Keyboard::Left:
				case sf::Keyboard::A:
					movement.left = true;
					break;

				case sf::Keyboard::Up:
				case sf::Keyboard::W:
					movement.up = true;
					break;

				case sf::Keyboard::Down:
				case sf::Keyboard::S:
					movement.down = true;
					break;

				case sf::Keyboard::RShift:
				case sf::Keyboard::LShift:
					movement.running = true;
					break;

				default:
					break;
			}
		}
	}

	void handleKeyUp(sf::Keyboard::Key key)
	{
		for (auto const &entity : mEntities) {
			auto &movement = mCoordinator->getComponent<MovementNew>(entity);

			switch (key) {
				case sf::Keyboard::Right:
				case sf::Keyboard::D:
					movement.right = false;
					break;

				case sf::Keyboard::Left:
				case sf::Keyboard::A:
					movement.left = false;
					break;

				case sf::Keyboard::Up:
				case sf::Keyboard::W:
					movement.up = false;
					break;

				case sf::Keyboard::Down:
				case sf::Keyboard::S:
					movement.down = false;
					break;

				case sf::Keyboard::RShift:
				case sf::Keyboard::LShift:
					movement.running = false;
					break;

				default:
					break;
			}
		}
	}

private:
	ecs::Coordinator *mCoordinator;
	Game *mGame;

	sf::Vector2f getCenter(Renderable &renderable, Transform &transform)
	{
		float xPos = transform.position.x + (renderable.size.x / 2);
		float yPos = transform.position.y + (renderable.size.y / 2);

		return sf::Vector2f(xPos, yPos);
	}
};

#endif
