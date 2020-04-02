#ifndef GAME_STATES_GAME_ECS_TEST_HPP
#define GAME_STATES_GAME_ECS_TEST_HPP

#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

#include "../ecs.hpp"
#include "../game_state.hpp"

struct Transform {
	sf::Vector2f position;
};

struct RigidBody {
	sf::Vector2f velocity;
	sf::Vector2f acceleration;
	sf::Vector2f deceleration;
};

struct MovementNew {
	bool up;
	bool right;
	bool down;
	bool left;
	bool running;
};

struct Renderable {
	sf::Color color;
};

struct Player {
	float maxWalkSpeed;
	float maxRunSpeed;
};

struct EntityComparator {
	bool operator()(const ecs::Entity &entity1,
			const ecs::Entity &entity2) const
	{
		auto const &transform1 =
		    sCoordinator->getComponent<Transform>(entity1);
		auto const &transform2 =
		    sCoordinator->getComponent<Transform>(entity2);

		if (transform1.position.y == transform2.position.y) {
			return transform1.position.x < transform2.position.x;
		}

		return transform1.position.y < transform2.position.y;
	}

	ecs::Coordinator *sCoordinator;
};

class RenderSystem : public ecs::System
{
public:
	void init(ecs::Coordinator *coordinator, Game *game)
	{
		mCoordinator = coordinator;
		mGame = game;
	}

	void update()
	{
		// I have no clue what a better way to sort sets would be.
		std::set<ecs::Entity, EntityComparator> srtd(
		    {}, EntityComparator{.sCoordinator = mCoordinator});
		for (auto &entity : mEntities) {
			srtd.insert(entity);
		}

		// Draw the sorted set.
		for (auto const &entity : srtd) {
			auto const &transform =
			    mCoordinator->getComponent<Transform>(entity);
			auto const &renderable =
			    mCoordinator->getComponent<Renderable>(entity);

			// Temporaraly draw a rectangle for the entities.
			sf::RectangleShape rect(sf::Vector2f(50.0f, 50.0f));
			rect.setPosition(transform.position);
			rect.setFillColor(renderable.color);
			mGame->window.draw(rect);
		}
	}

private:
	ecs::Coordinator *mCoordinator;
	Game *mGame;
};

class PlayerSystem : public ecs::System
{
public:
	void init(ecs::Coordinator *coordinator, Game *game)
	{
		mCoordinator = coordinator;
		mGame = game;
	}

	void update()
	{
		for (auto const &entity : mEntities) {
			auto &transform =
			    mCoordinator->getComponent<Transform>(entity);
			auto &rigidBody =
			    mCoordinator->getComponent<RigidBody>(entity);
			auto &movement =
			    mCoordinator->getComponent<MovementNew>(entity);
			auto &player =
			    mCoordinator->getComponent<Player>(entity);

			if (movement.running) {
				if (movement.right &&
				    rigidBody.velocity.x < player.maxRunSpeed) {
					rigidBody.velocity.x +=
					    rigidBody.acceleration.x;
				}

				if (movement.left && rigidBody.velocity.x >
							 -player.maxRunSpeed) {
					rigidBody.velocity.x -=
					    rigidBody.acceleration.x;
				}

				if (movement.up && rigidBody.velocity.y >
						       -player.maxRunSpeed) {
					rigidBody.velocity.y -=
					    rigidBody.acceleration.y;
				}

				if (movement.down &&
				    rigidBody.velocity.y < player.maxRunSpeed) {
					rigidBody.velocity.y +=
					    rigidBody.acceleration.y;
				}
			} else {
				if (movement.right && rigidBody.velocity.x <
							  player.maxWalkSpeed) {
					rigidBody.velocity.x +=
					    rigidBody.acceleration.x;
				}

				if (movement.left && rigidBody.velocity.x >
							 -player.maxWalkSpeed) {
					rigidBody.velocity.x -=
					    rigidBody.acceleration.x;
				}

				if (movement.up && rigidBody.velocity.y >
						       -player.maxWalkSpeed) {
					rigidBody.velocity.y -=
					    rigidBody.acceleration.y;
				}

				if (movement.down && rigidBody.velocity.y <
							 player.maxWalkSpeed) {
					rigidBody.velocity.y +=
					    rigidBody.acceleration.y;
				}

				// Clamp the run speed.
				if (rigidBody.velocity.x > 0) {
					rigidBody.velocity.x =
					    std::min(player.maxWalkSpeed,
						     rigidBody.velocity.x);
				}

				if (rigidBody.velocity.y > 0) {
					rigidBody.velocity.y =
					    std::min(player.maxWalkSpeed,
						     rigidBody.velocity.y);
				}

				if (rigidBody.velocity.x < 0) {
					rigidBody.velocity.x =
					    std::max(-player.maxWalkSpeed,
						     rigidBody.velocity.x);
				}

				if (rigidBody.velocity.y < 0) {
					rigidBody.velocity.y =
					    std::max(-player.maxWalkSpeed,
						     rigidBody.velocity.y);
				}
			}

			// Apply deceleration on X
			if (rigidBody.velocity.x > 0 && !movement.right) {
				if (rigidBody.velocity.x <
				    rigidBody.deceleration.x) {
					rigidBody.velocity.x = 0;
				} else {
					rigidBody.velocity.x -=
					    rigidBody.deceleration.x;
				}
			} else if (rigidBody.velocity.x < 0 && !movement.left) {
				if (rigidBody.velocity.x >
				    rigidBody.deceleration.x) {
					rigidBody.velocity.x = 0;
				} else {
					rigidBody.velocity.x +=
					    rigidBody.deceleration.x;
				}
			}
			transform.position.x += rigidBody.velocity.x;

			// Apply acceleration on Y
			if (rigidBody.velocity.y > 0 && !movement.down) {
				if (rigidBody.velocity.y <
				    rigidBody.deceleration.y) {
					rigidBody.velocity.y = 0;
				} else {
					rigidBody.velocity.y -=
					    rigidBody.deceleration.y;
				}
			} else if (rigidBody.velocity.y < 0 && !movement.up) {
				if (rigidBody.velocity.y >
				    rigidBody.deceleration.y) {
					rigidBody.velocity.y = 0;
				} else {
					rigidBody.velocity.y +=
					    rigidBody.deceleration.y;
				}
			}
			transform.position.y += rigidBody.velocity.y;
		}
	}

	void handleKeyDown(sf::Keyboard::Key key)
	{
		for (auto const &entity : mEntities) {
			auto &movement =
			    mCoordinator->getComponent<MovementNew>(entity);

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
			auto &movement =
			    mCoordinator->getComponent<MovementNew>(entity);

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
};

class GameEcsTest : public GameState
{
public:
	GameEcsTest(Game *game)
	{
		this->game = game;

		sf::Vector2f pos = sf::Vector2f(this->game->window.getSize());
		mGameView.setSize(pos);
		pos *= 0.5f;
		mGameView.setCenter(pos);

		// Initialize the coordinator.
		mCoordinator.init();

		// Register components
		mCoordinator.registerComponent<Player>();
		mCoordinator.registerComponent<Transform>();
		mCoordinator.registerComponent<Renderable>();
		mCoordinator.registerComponent<RigidBody>();
		mCoordinator.registerComponent<MovementNew>();

		// Render System
		mRenderSystem = mCoordinator.registerSystem<RenderSystem>();
		{
			ecs::Signature signature;

			// Add components
			signature.set(
			    mCoordinator.getComponentType<Transform>());
			signature.set(
			    mCoordinator.getComponentType<Renderable>());

			mCoordinator.setSystemSignature<RenderSystem>(
			    signature);
		}
		mRenderSystem->init(&mCoordinator, game);

		// Player System
		mPlayerSystem = mCoordinator.registerSystem<PlayerSystem>();
		{
			ecs::Signature signature;

			// Add components
			signature.set(mCoordinator.getComponentType<Player>());
			signature.set(
			    mCoordinator.getComponentType<Transform>());
			signature.set(
			    mCoordinator.getComponentType<RigidBody>());
			signature.set(
			    mCoordinator.getComponentType<MovementNew>());

			mCoordinator.setSystemSignature<PlayerSystem>(
			    signature);
		}
		mPlayerSystem->init(&mCoordinator, game);

		// Create a test entity that is controllable.
		// clang-format off
		ecs::Entity entity = mCoordinator.createEntity();
		mCoordinator.addComponent(entity, Player{
			.maxWalkSpeed = 2.0f,
			.maxRunSpeed = 4.0f,
		});
		mCoordinator.addComponent(entity, Transform{
			.position = sf::Vector2f(0.0f, 0.0f),
		});
		mCoordinator.addComponent(entity, Renderable{
			.color = sf::Color::White
		});
		mCoordinator.addComponent(entity, MovementNew{
			.up = false,
			.right = false,
			.down = false,
			.left = false,
			.running = false,
		});
		mCoordinator.addComponent(entity, RigidBody{
			.velocity = sf::Vector2f(0.0f, 0.0f),
			.acceleration = sf::Vector2f(0.1f, 0.1f),
			.deceleration = sf::Vector2f(0.25f, 0.25f),
		});
		mEntities.push_back(entity);
		// clang-format on

		// Create another test entity.
		// clang-format off
		ecs::Entity entityTwo = mCoordinator.createEntity();
		mCoordinator.addComponent(entityTwo, Transform{
			.position = sf::Vector2f(1.0f, 15.0f)
		});
		mCoordinator.addComponent(entityTwo, Renderable{
			.color = sf::Color::Red
		});
		mEntities.push_back(entityTwo);
		// clang-format on
	}

	virtual void draw(const sf::Time deltaTime)
	{
		this->game->window.clear(sf::Color::Black);
		this->game->window.setView(this->mGameView);

		// Updating the render system draws it.
		mRenderSystem->update();
	}

	virtual void update(const sf::Time deltaTime)
	{
		mPlayerSystem->update();
	}

	virtual void handleInput()
	{
		sf::Event event;

		while (this->game->window.pollEvent(event)) {
			switch (event.type) {
				case sf::Event::Closed:
					this->game->window.close();

					break;

				case sf::Event::Resized:
					this->mGameView.setSize(
					    event.size.width,
					    event.size.height);

					break;

				case sf::Event::KeyPressed:
					if (event.key.code ==
					    sf::Keyboard::Escape) {
						this->game->window.close();
					}

					mPlayerSystem->handleKeyDown(
					    event.key.code);
					break;

				case sf::Event::KeyReleased:
					mPlayerSystem->handleKeyUp(
					    event.key.code);
					break;

				default:
					break;
			}
		}
	}

private:
	sf::View mGameView;

	ecs::Coordinator mCoordinator;
	std::shared_ptr<RenderSystem> mRenderSystem;
	std::shared_ptr<PlayerSystem> mPlayerSystem;
	std::vector<ecs::Entity> mEntities;
};

#endif
