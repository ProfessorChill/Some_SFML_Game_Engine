#include "entity.hpp"

Entity::Entity(sf::Texture *texture, unsigned int id)
    : pos(sf::Vector2f(0, 0)), texture(texture)
{
	this->id = id;

	this->animations["idle_up"] = Animation();
	this->animations["idle_up"].setSpriteSheet(*this->texture);
	this->animations["idle_up"].addFrame(sf::IntRect(0, 0, 65, 65));

	this->animations["idle_left"] = Animation();
	this->animations["idle_left"].setSpriteSheet(*this->texture);
	this->animations["idle_left"].addFrame(sf::IntRect(0, 64, 65, 65));

	this->animations["idle_down"] = Animation();
	this->animations["idle_down"].setSpriteSheet(*this->texture);
	this->animations["idle_down"].addFrame(sf::IntRect(0, 128, 65, 65));

	this->animations["idle_right"] = Animation();
	this->animations["idle_right"].setSpriteSheet(*this->texture);
	this->animations["idle_right"].addFrame(sf::IntRect(0, 192, 65, 65));

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

	/*
	if (this->displayCollision) {
		sf::RectangleShape shape = sf::RectangleShape(
		    sf::Vector2f(this->collRect.width, this->collRect.height));
		shape.setFillColor(sf::Color(255, 0, 0, 100));
		shape.setPosition(this->collRect.left, this->collRect.top);
		window.draw(shape);
	}
	*/
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
	} else {
		if (this->movementRight && this->vel.x < this->maxWalkSpeed) {
			this->vel.x += this->acceleration;
		}
	}

	if (this->running) {
		if (this->movementLeft && this->vel.x > -this->maxRunSpeed) {
			this->vel.x -= this->acceleration;
		}
	} else {
		if (this->movementLeft && this->vel.x > -this->maxWalkSpeed) {
			this->vel.x -= this->acceleration;
		}
	}

	if (this->running) {
		if (this->movementUp && this->vel.y > -this->maxRunSpeed) {
			this->vel.y -= this->acceleration;
		}
	} else {
		if (this->movementUp && this->vel.y > -this->maxWalkSpeed) {
			this->vel.y -= this->acceleration;
		}
	}

	if (this->running) {
		if (this->movementDown && this->vel.y < this->maxRunSpeed) {
			this->vel.y += this->acceleration;
		}
	} else {
		if (this->movementDown && this->vel.y < this->maxWalkSpeed) {
			this->vel.y += this->acceleration;
		}
	}

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

	if (this->vel.y > 0 && !this->movementDown) {
		if (this->vel.y < this->deceleration) {
			this->vel.y = 0;
		} else {
			this->vel.y -= this->deceleration;
		}
	} else if (this->vel.y < 0 && !this->movementUp) {
		if (this->vel.y > this->deceleration) {
			this->vel.x = 0;
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

	this->setPosition(this->pos);
	this->sprite.setPosition(this->pos);
	this->sprite.update(deltaTime);
}

void Entity::keyPress(sf::Keyboard::Key &key)
{
	switch (key) {
	case sf::Keyboard::D:
		this->movementRight = true;
		this->currentAnimation = &this->animations["idle_right"];
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
