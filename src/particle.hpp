#ifndef PARTICLE_HPP
#define PARTICLE_HPP

#include <SFML/Graphics.hpp>
#include <list>
#include <memory>
#include <random>
#include <sstream>
#include <vector>

namespace Shape
{
enum { CIRCLE, SQUARE };
}

struct Particle : public sf::Drawable {
	sf::Vertex drawVertex;
	sf::Vector2f vel;

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const
	{
		target.draw(&drawVertex, 1, sf::Points, states);
	}
};

class ParticleSystem : public sf::Drawable
{
public:
	ParticleSystem(sf::Vector2u canvasSize);
	~ParticleSystem();

	int getDissolutionRate() const
	{
		return this->dissolutionRate;
	}

	int getNumberOfParticles() const
	{
		return this->particles.size();
	}

	float getParticleSpeed() const
	{
		return this->particleSpeed;
	}
	std::string getNumberOfParticlesString() const;

	void setCanvasSize(sf::Vector2f newSize)
	{
		this->canvasSize = newSize;
	}

	void setDissolutionRate(sf::Uint8 rate)
	{
		this->dissolutionRate = rate;
	}

	void setDissolve()
	{
		this->dissolve = !this->dissolve;
	}
	void setDistibution()
	{
		this->shape = (this->shape + 1) % 2;
	}

	void setGravity(float x, float y)
	{
		this->gravity.x = x;
		this->gravity.y = y;
	}

	void setGravity(sf::Vector2f gravity)
	{
		this->gravity = gravity;
	}
	void setParticleSpeed(float speed)
	{
		this->particleSpeed = speed;
	}

	void setPosition(float x, float y)
	{
		this->startPos.x = x;
		this->startPos.y = y;
	}

	void setPosition(sf::Vector2f position)
	{
		this->startPos = position;
	}
	void setShape(sf::Uint8 shape)
	{
		this->shape = shape;
	}

	virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;

	void fuel(int particles);
	void update(float dt);

private:
	bool dissolve;
	float particleSpeed;

	sf::Color transparent;

	sf::Uint8 dissolutionRate;
	sf::Uint8 shape;

	sf::Vector2f gravity;
	sf::Vector2f startPos;
	sf::Vector2f canvasSize;

	std::vector<std::shared_ptr<Particle>> particles;
};

#endif
