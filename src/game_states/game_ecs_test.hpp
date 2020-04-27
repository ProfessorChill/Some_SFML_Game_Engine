#ifndef GAME_STATES_GAME_ECS_TEST_HPP
#define GAME_STATES_GAME_ECS_TEST_HPP

#include <SFML/Graphics/Color.hpp>
#include <cmath>
#include <iostream>
#include <memory>
#include <vector>

#include "../ecs.hpp"
#include "../game_state.hpp"
#include "../tmx-parser/map.hpp"

#include "../systems/player_system.hpp"
#include "../systems/render_system.hpp"
#include "../systems/rigid_physics_system.hpp"

struct Player;	    // ../components/player.hpp
struct MovementNew; // ../components/movement.hpp
struct Renderable;  // ../components/renderable.hpp
struct RigidBody;   // ../components/rigidbody.hpp
struct Transform;   // ../components/transform.hpp

class GameEcsTest : public GameState
{
public:
	GameEcsTest(Game *game)
	{
		this->game = game;

		this->map = tmx::Map(".", "resources/maps/untitled.tmx");

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
			signature.set(mCoordinator.getComponentType<Transform>());
			signature.set(mCoordinator.getComponentType<Renderable>());

			mCoordinator.setSystemSignature<RenderSystem>(signature);
		}
		mRenderSystem->init(&mCoordinator, game);

		// Player System
		mPlayerSystem = mCoordinator.registerSystem<PlayerSystem>();
		{
			ecs::Signature signature;

			// Add components
			signature.set(mCoordinator.getComponentType<Player>());
			signature.set(mCoordinator.getComponentType<Transform>());
			signature.set(mCoordinator.getComponentType<RigidBody>());
			signature.set(mCoordinator.getComponentType<MovementNew>());
			signature.set(mCoordinator.getComponentType<Renderable>());

			mCoordinator.setSystemSignature<PlayerSystem>(signature);
		}
		mPlayerSystem->init(&mCoordinator, game);

		// Rigid Physics System
		mRigidPhysicsSystem = mCoordinator.registerSystem<RigidPhysicsSystem>();
		{
			ecs::Signature signature;

			signature.set(mCoordinator.getComponentType<RigidBody>());
			signature.set(mCoordinator.getComponentType<Transform>());
			signature.set(mCoordinator.getComponentType<Renderable>());

			mCoordinator.setSystemSignature<RigidPhysicsSystem>(signature);
		}
		mRigidPhysicsSystem->init(&mCoordinator, game);

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
			.color = sf::Color::White,
			.size = sf::Vector2f(32.0f, 32.0f),
		});
		mCoordinator.addComponent(entity, MovementNew{
			.up = false,
			.right = false,
			.down = false,
			.left = false,
			.running = false,
			.mapPtr = &this->map,
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
			.position = sf::Vector2f(1.0f, 15.0f),
		});
		mCoordinator.addComponent(entityTwo, Renderable{
			.color = sf::Color::Red,
			.size = sf::Vector2f(32.0f, 32.0f),
		});
		mCoordinator.addComponent(entityTwo, RigidBody{
			.velocity = sf::Vector2f(0.0f, 0.0f),
			.acceleration = sf::Vector2f(0.1f, 0.1f),
			.deceleration = sf::Vector2f(0.25f, 0.25f),
		});
		mEntities.push_back(entityTwo);
		// clang-format on

		// Initialize spawns at the end of the constructor
		mPlayerSystem->initSpawns();
	}

	virtual void draw(const sf::Time deltaTime)
	{
		this->game->window.clear(sf::Color::Black);
		this->game->window.setView(this->mGameView);

		sf::Vector2f viewportTopLeft = this->game->window.mapPixelToCoords(
		    sf::Vector2i(0, 0), this->game->window.getView());
		sf::Vector2f viewportSize = this->game->window.getView().getSize();
		sf::Rect<float> viewport(viewportTopLeft, viewportSize);

		// Updating the render system draws it.
		// We pass the map so we can draw/sort the map as well.
		mRenderSystem->draw(&this->map, sf::Clock().restart(), viewport);
	}

	virtual void update(const sf::Time deltaTime)
	{
		mPlayerSystem->update(&mGameView);
		mRigidPhysicsSystem->update(&map);
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
					this->mGameView.setSize(event.size.width,
								event.size.height);

					break;

				case sf::Event::KeyPressed:
					if (event.key.code == sf::Keyboard::Escape) {
						this->game->window.close();
					}

					mPlayerSystem->handleKeyDown(event.key.code);
					break;

				case sf::Event::KeyReleased:
					mPlayerSystem->handleKeyUp(event.key.code);
					break;

				default:
					break;
			}
		}
	}

public:
	sf::View mGameView;

private:
	tmx::Map map;

	ecs::Coordinator mCoordinator;
	std::shared_ptr<RenderSystem> mRenderSystem;
	std::shared_ptr<PlayerSystem> mPlayerSystem;
	std::shared_ptr<RigidPhysicsSystem> mRigidPhysicsSystem;
	std::vector<ecs::Entity> mEntities;
};

#endif
