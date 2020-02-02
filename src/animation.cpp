#include <iostream>

#include "animation.hpp"

Animation::Animation() : texture(nullptr) {}

void Animation::addFrame(sf::IntRect rect)
{
	// Add frame to animation

	this->frames.push_back(rect);
}

void Animation::setSpriteSheet(const sf::Texture &texture)
{
	// Change the spritesheet

	this->texture = &texture;
}

const sf::Texture *Animation::getSpriteSheet() const
{
	// Get the current spritesheet

	return this->texture;
}

std::size_t Animation::getSize() const
{
	// Get the amount of frames in the animation

	return this->frames.size();
}

const sf::IntRect &Animation::getFrame(std::size_t n) const
{
	// Get the specified frame

	return this->frames[n];
}
