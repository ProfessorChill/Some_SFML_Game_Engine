#include "particle.hpp"

ParticleSystem::ParticleSystem(sf::Vector2u canvasSize)
    : dissolve(false), particleSpeed(100.0f),
      transparent(sf::Color(0, 0, 0, 0)), dissolutionRate(4),
      shape(Shape::CIRCLE), gravity(sf::Vector2f(0.0f, 0.0f)),
      startPos(sf::Vector2f(static_cast<float>(canvasSize.x) / 2,
			    static_cast<float>(canvasSize.y) / 2)),
      canvasSize(canvasSize)
{
}

ParticleSystem::~ParticleSystem() { this->particles.clear(); }

void ParticleSystem::draw(sf::RenderTarget &target,
			  sf::RenderStates states) const
{
	for (const std::shared_ptr<Particle> &item : this->particles) {
		target.draw(&item.get()->drawVertex, 1, sf::Points);
	}
}

void ParticleSystem::fuel(int particles)
{
	for (int i = 0; i < particles; i++) {
		Particle *particle;
		particle = new Particle;
		particle->drawVertex.position.x = this->startPos.x;
		particle->drawVertex.position.y = this->startPos.y;

		std::random_device rd;
		std::mt19937 gen(rd());

		switch (this->shape) {
		case Shape::CIRCLE: {
			std::uniform_real_distribution<> randomAngle(
			    0.0f, (2.0f * 3.14159265));
			float angle = randomAngle(gen);

			std::uniform_real_distribution<> randomAngleCos(
			    0.0f, cos(angle));
			std::uniform_real_distribution<> randomAngleSin(
			    0.0f, sin(angle));
			particle->vel.x = randomAngleCos(gen);
			particle->vel.y = randomAngleSin(gen);

			break;
		}

		case Shape::SQUARE: {
			std::uniform_real_distribution<> randomSide(-1.0f,
								    1.0f);
			particle->vel.x = randomSide(gen);
			particle->vel.y = randomSide(gen);

			break;
		}

		default: {
			particle->vel.x = 0.5f;
			particle->vel.y = 0.5f;
		}
		}

		if (particle->vel.x == 0.0f && particle->vel.y == 0.0f) {
			delete particle;
			continue;
		}

		std::uniform_int_distribution<> randomColor(0, 255);
		particle->drawVertex.color.r = randomColor(gen);
		particle->drawVertex.color.g = randomColor(gen);
		particle->drawVertex.color.b = randomColor(gen);
		particle->drawVertex.color.a = 255;

		this->particles.push_back(std::shared_ptr<Particle>(particle));
	}
}

const std::string ParticleSystem::getNumberOfParticlesString() const
{
	std::ostringstream oss;
	oss << this->particles.size();
	return oss.str();
}

void ParticleSystem::update(float dt)
{
	for (auto it = this->particles.begin(); it != this->particles.end();
	     it++) {
		(*it)->vel.x += this->gravity.x * dt;
		(*it)->vel.y += this->gravity.y * dt;

		(*it)->drawVertex.position.x +=
		    (*it)->vel.x * dt * this->particleSpeed;
		(*it)->drawVertex.position.y +=
		    (*it)->vel.y * dt * this->particleSpeed;

		if (this->dissolve) {
			(*it)->drawVertex.color.a -= this->dissolutionRate;
		}

		if ((*it)->drawVertex.position.x > this->canvasSize.x ||
		    (*it)->drawVertex.position.x < 0 ||
		    (*it)->drawVertex.position.y > this->canvasSize.y ||
		    (*it)->drawVertex.position.y < 0 ||
		    (*it)->drawVertex.color.a < 10) {
			it = this->particles.erase(it);

			if (it == this->particles.end()) {
				return;
			}
		}
	}
}
