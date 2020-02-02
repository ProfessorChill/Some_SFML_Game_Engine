#ifndef ANIMATED_SPRITE_HPP
#define ANIMATED_SPRITE_HPP

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Time.hpp>
#include <SFML/System/Vector2.hpp>
#include <iostream>

#include "animation.hpp"

class AnimatedSprite : public sf::Drawable, public sf::Transformable
{
    public:
	explicit AnimatedSprite(sf::Time frameTime = sf::seconds(0.2f),
				bool paused = false, bool loop = true);

	void update(sf::Time deltaTime);
	void setAnimation(const Animation &animation);
	void setFrameTime(sf::Time time);
	void play();
	void play(const Animation &animation);
	void pause();
	void stop();
	void setLooped(bool looped);
	void setColor(const sf::Color &color);
	const Animation *getAnimation() const;
	sf::FloatRect getLocalBounds() const;
	sf::FloatRect getGlobalBounds() const;
	bool isLooping() const;
	bool isPlaying() const;
	sf::Time getFrameTime() const;
	void setFrame(std::size_t newFrame, bool resetTime = true);

    private:
	const Animation *animation;
	sf::Time frameTime;
	sf::Time currentTime;
	std::size_t currentFrame;
	bool isPaused;
	bool isLooped;
	const sf::Texture *texture;
	sf::Vertex vertices[4];

	virtual void draw(sf::RenderTarget &target,
			  sf::RenderStates states) const;
};

#endif
