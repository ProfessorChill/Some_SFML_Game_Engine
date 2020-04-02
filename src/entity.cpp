#include "entity.hpp"

Entity::Entity(sf::Texture *texture, unsigned int id)
    : pos(sf::Vector2f(0, 0)), texture(texture)
{
	this->id = id;

	this->animations["idle_up"] = Animation();
	this->animations["idle_up"].setSpriteSheet(*this->texture);
	this->animations["idle_up"].addFrame(sf::IntRect(0, 0 + 15, 65, 50));

	this->animations["idle_left"] = Animation();
	this->animations["idle_left"].setSpriteSheet(*this->texture);
	this->animations["idle_left"].addFrame(sf::IntRect(0, 64 + 15, 65, 50));

	this->animations["idle_down"] = Animation();
	this->animations["idle_down"].setSpriteSheet(*this->texture);
	this->animations["idle_down"].addFrame(
	    sf::IntRect(0, 128 + 15, 65, 50));

	this->animations["idle_right"] = Animation();
	this->animations["idle_right"].setSpriteSheet(*this->texture);
	this->animations["idle_right"].addFrame(
	    sf::IntRect(0, 192 + 15, 65, 50));

	this->currentAnimation = &this->animations["idle_down"];

	this->movementUp = false;
	this->movementRight = false;
	this->movementDown = false;
	this->movementLeft = false;
	this->running = false;

	this->sprite = AnimatedSprite(sf::seconds(0.5), true, false);
	this->sprite.setPosition(this->pos);

	// this->collRect = this->sprite.getGlobalBounds();
}

void Entity::draw(sf::RenderWindow &window, const sf::Time deltaTime)
{
	window.draw(this->sprite);

	if (this->displayCollision) {
		sf::RectangleShape shape = sf::RectangleShape(
		    sf::Vector2f(this->collRect.width, this->collRect.height));
		shape.setFillColor(sf::Color(255, 0, 0, 100));
		shape.setPosition(this->collRect.left, this->collRect.top);
		window.draw(shape);
	}
}

void Entity::update(const sf::Time deltaTime)
{
	// Do some update things here
	float dt = 1.0f + deltaTime.asSeconds();

	this->lastPos = this->pos;

	// Handle velocity
	if (this->running) {
		if (this->movementRight && this->vel.x < this->maxRunSpeed) {
			this->vel.x += this->acceleration;
		}

		if (this->movementLeft && this->vel.x > -this->maxRunSpeed) {
			this->vel.x -= this->acceleration;
		}

		if (this->movementUp && this->vel.y > -this->maxRunSpeed) {
			this->vel.y -= this->acceleration;
		}

		if (this->movementDown && this->vel.y < this->maxRunSpeed) {
			this->vel.y += this->acceleration;
		}
	} else {
		if (this->movementRight && this->vel.x < this->maxWalkSpeed) {
			this->vel.x += this->acceleration;
		}

		if (this->movementLeft && this->vel.x > -this->maxWalkSpeed) {
			this->vel.x -= this->acceleration;
		}

		if (this->movementUp && this->vel.y > -this->maxWalkSpeed) {
			this->vel.y -= this->acceleration;
		}

		if (this->movementDown && this->vel.y < this->maxWalkSpeed) {
			this->vel.y += this->acceleration;
		}

		// Clamp the run speed
		if (this->vel.x > 0) {
			this->vel.x = std::min(this->maxWalkSpeed, this->vel.x);
		}

		if (this->vel.y > 0) {
			this->vel.y = std::min(this->maxWalkSpeed, this->vel.y);
		}

		if (this->vel.x < 0) {
			this->vel.x =
			    std::max(-this->maxWalkSpeed, this->vel.x);
		}

		if (this->vel.y < 0) {
			this->vel.y =
			    std::max(-this->maxWalkSpeed, this->vel.y);
		}
	}

	// Apply deceleration on X
	if (this->vel.x > 0 && !this->movementRight) {
		if (this->vel.x < this->deceleration) {
			this->vel.x = 0;
		} else {
			this->vel.x -= this->deceleration;
		}
	} else if (this->vel.x < 0 && !this->movementLeft) {
		if (this->vel.x > this->deceleration) {
			this->vel.x = 0;
		} else {
			this->vel.x += this->deceleration;
		}
	}
	this->pos.x += this->vel.x * dt;

	// Apply acceleration on Y
	if (this->vel.y > 0 && !this->movementDown) {
		if (this->vel.y < this->deceleration) {
			this->vel.y = 0;
		} else {
			this->vel.y -= this->deceleration;
		}
	} else if (this->vel.y < 0 && !this->movementUp) {
		if (this->vel.y > this->deceleration) {
			this->vel.y = 0;
		} else {
			this->vel.y += this->deceleration;
		}
	}
	this->pos.y += this->vel.y * dt;

	this->sprite.play(*this->currentAnimation);

	// Update the collision rectangle
	this->collRect = this->sprite.getGlobalBounds();
	this->collRect.width /= 2;
	this->collRect.left += this->collRect.width / 2;
	this->collRect.top +=
	    this->collRect.height - (this->collRect.height / 4);
	this->collRect.height /= 4;

	// Apply the position params
	this->setPosition(this->pos);
	this->sprite.setPosition(this->pos);
	this->sprite.update(deltaTime);
}

void Entity::keyPress(sf::Keyboard::Key &key)
{
	switch (key) {
		case sf::Keyboard::D:
			this->movementRight = true;
			this->currentAnimation =
			    &this->animations["idle_right"];
			break;

		case sf::Keyboard::A:
			this->movementLeft = true;
			this->currentAnimation = &this->animations["idle_left"];
			break;

		case sf::Keyboard::W:
			this->movementUp = true;
			this->currentAnimation = &this->animations["idle_up"];
			break;

		case sf::Keyboard::S:
			this->movementDown = true;
			this->currentAnimation = &this->animations["idle_down"];
			break;

		case sf::Keyboard::LShift:
			this->running = true;
			break;

		default:
			break;
	}
}

void Entity::keyRelease(sf::Keyboard::Key &key)
{
	switch (key) {
		case sf::Keyboard::D:
			this->movementRight = false;
			break;

		case sf::Keyboard::A:
			this->movementLeft = false;
			break;

		case sf::Keyboard::W:
			this->movementUp = false;
			break;

		case sf::Keyboard::S:
			this->movementDown = false;
			break;

		case sf::Keyboard::LShift:
			this->running = false;
			break;

		default:
			break;
	}
}

float clamp(float value, float min, float max)
{
	return std::max(min, std::min(max, value));
}

sf::Vector2f clamp(sf::Vector2f value, sf::Vector2f min, sf::Vector2f max)
{
	float clampX = std::max(min.x, std::min(max.x, value.x));
	float clampY = std::max(min.y, std::min(max.y, value.y));

	return sf::Vector2f(clampX, clampY);
}

float vecLength(sf::Vector2f value)
{
	float x = std::pow(value.x, 2);
	float y = std::pow(value.y, 2);

	return std::sqrt(x + y);
}

float vecDot(sf::Vector2f one, sf::Vector2f two)
{
	return one.x * two.x + one.y * two.y;
}

sf::Vector2f vecNormalize(sf::Vector2f v)
{
	float len = vecLength(v);

	return sf::Vector2f(v.x / len, v.y / len);
}

Direction vectorDirection(sf::Vector2f target)
{
	sf::Vector2f compass[] = {
	    sf::Vector2f(0.0f, 1.0f),  // Up
	    sf::Vector2f(1.0f, 0.0f),  // Right
	    sf::Vector2f(0.0f, -1.0f), // Down
	    sf::Vector2f(-1.0f, 0.0f), // Left
	};

	float max = 0.0f;
	int best_match = -1;

	for (int i = 0; i < 4; i++) {
		float dot_product = vecDot(vecNormalize(target), compass[i]);

		if (dot_product > max) {
			max = dot_product;
			best_match = i;
		}
	}

	return (Direction)best_match;
}

void Entity::checkCollision(Entity *other)
{
	sf::Vector2f center(this->collRect.left + (this->collRect.width / 2),
			    this->collRect.top + (this->collRect.height / 2));
	sf::Vector2f aabb_half_extents(other->collRect.width / 2,
				       other->collRect.height / 2);
	sf::Vector2f aabb_center(other->collRect.left + aabb_half_extents.x,
				 other->collRect.top + aabb_half_extents.y);

	sf::Vector2f difference = center - aabb_center;
	sf::Vector2f clamped =
	    clamp(difference, -aabb_half_extents, aabb_half_extents);
	sf::Vector2f closest = aabb_center + clamped;
	difference = closest - center;

	if (vecLength(difference) <= this->collRect.width / 2) {
		Direction dir = vectorDirection(difference);

		if (dir == LEFT || dir == RIGHT) {
			this->vel.x = 0;

			float penetration =
			    (this->collRect.width / 2) - std::abs(difference.x);

			if (dir == LEFT) {
				this->pos.x += penetration;
			} else {
				this->pos.x -= penetration;
			}
		} else {
			this->vel.y = 0;

			float penetration =
			    (this->collRect.height) - std::abs(difference.y);

			if (dir == UP) {
				this->pos.y -= penetration;
			} else {
				this->pos.y += penetration;
			}
		}

		this->setPosition(this->pos);
		this->sprite.setPosition(this->pos);
	}
}
