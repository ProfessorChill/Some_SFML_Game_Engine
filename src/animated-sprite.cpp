#include "animated-sprite.hpp"

AnimatedSprite::AnimatedSprite(sf::Time frameTime, bool paused, bool looped)
    : animation(nullptr), frameTime(frameTime), currentFrame(0),
      isPaused(paused), isLooped(looped), texture(nullptr)
{
}

void AnimatedSprite::setAnimation(const Animation &animation)
{
	this->animation = &animation;
	this->texture = this->animation->getSpriteSheet();
	this->currentFrame = 0;
	this->setFrame(this->currentFrame);
}

void AnimatedSprite::setFrameTime(sf::Time time)
{
	// Set the animated sprite frame time

	this->frameTime = time;
}

void AnimatedSprite::play()
{
	// Sets the animation to playing

	this->isPaused = false;
}

void AnimatedSprite::play(const Animation &animation)
{
	// Changes the animation and plays the set animation

	if (this->getAnimation() != &animation) {
		this->setAnimation(animation);
	}

	this->play();
}

void AnimatedSprite::pause()
{
	// Pauses the animation

	this->isPaused = true;
}

void AnimatedSprite::stop()
{
	this->isPaused = true;
	this->currentFrame = 0;
	this->setFrame(this->currentFrame);
}

void AnimatedSprite::setLooped(bool looped)
{
	// Sets if the sprite is looped or not

	this->isLooped = looped;
}

void AnimatedSprite::setColor(const sf::Color &color)
{
	// I do not know what this does :(
	// Maybe it's an overlay color...

	this->vertices[0].color = color;
	this->vertices[1].color = color;
	this->vertices[2].color = color;
	this->vertices[3].color = color;
}

const Animation *AnimatedSprite::getAnimation() const
{
	// Returns the current animation

	return this->animation;
}

sf::FloatRect AnimatedSprite::getLocalBounds() const
{
	sf::IntRect rect = this->animation->getFrame(this->currentFrame);

	float width = static_cast<float>(std::abs(rect.width));
	float height = static_cast<float>(std::abs(rect.height));

	return sf::FloatRect(0.f, 0.f, width, height);
}

sf::FloatRect AnimatedSprite::getGlobalBounds() const
{
	// Get the global bounds of the animation

	return this->getTransform().transformRect(this->getLocalBounds());
}

bool AnimatedSprite::isLooping() const
{
	// Returns if the animation is looping or not

	return this->isLooped;
}

bool AnimatedSprite::isPlaying() const
{
	// Returns if the animation is playing or not

	return !this->isPaused;
}

sf::Time AnimatedSprite::getFrameTime() const
{
	// Gets the current frame time of the animation

	return this->frameTime;
}

void AnimatedSprite::setFrame(std::size_t newFrame, bool resetTime)
{
	if (this->animation) {
		sf::IntRect rect = this->animation->getFrame(newFrame);

		this->vertices[0].position = sf::Vector2f(0.f, 0.f);
		this->vertices[1].position =
		    sf::Vector2f(0.f, static_cast<float>(rect.height));
		this->vertices[2].position =
		    sf::Vector2f(static_cast<float>(rect.width),
				 static_cast<float>(rect.height));
		this->vertices[3].position =
		    sf::Vector2f(static_cast<float>(rect.width), 0.f);

		float left = static_cast<float>(rect.left) + 0.0001f;
		float right = left + static_cast<float>(rect.width);
		float top = static_cast<float>(rect.top);
		float bottom = top + static_cast<float>(rect.height);

		this->vertices[0].texCoords = sf::Vector2f(left, top);
		this->vertices[1].texCoords = sf::Vector2f(left, bottom);
		this->vertices[2].texCoords = sf::Vector2f(right, bottom);
		this->vertices[3].texCoords = sf::Vector2f(right, top);
	}

	if (resetTime) {
		this->currentTime = sf::Time::Zero;
	}
}

void AnimatedSprite::update(sf::Time deltaTime)
{
	if (!this->isPaused && this->animation) {
		this->currentTime += deltaTime;

		if (this->currentTime >= this->frameTime) {
			this->currentTime = sf::microseconds(
			    this->currentTime.asMicroseconds() %
			    this->frameTime.asMicroseconds());

			if (this->currentFrame + 1 <
			    this->animation->getSize()) {
				this->currentFrame++;
			} else {
				this->currentFrame = 0;

				if (!this->isLooped) {
					this->isPaused = true;
				}
			}

			this->setFrame(this->currentFrame, false);
		}
	}
}

void AnimatedSprite::draw(sf::RenderTarget &target,
			  sf::RenderStates states) const
{
	if (this->animation && this->texture) {
		states.transform *= this->getTransform();
		states.texture = this->texture;
		target.draw(this->vertices, 4, sf::Quads, states);
	}
}
