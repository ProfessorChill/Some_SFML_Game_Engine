#ifndef PHYSICS_AABB_HPP
#define PHYSICS_AABB_HPP

#include <SFML/Graphics/Rect.hpp>
#include <SFML/System/Vector2.hpp>
#include <algorithm>
#include <iostream>
#include <limits>
#include <math.h>

#include "../components/renderable.hpp"
#include "../components/rigidbody.hpp"
#include "../components/transform.hpp"

namespace physics
{
enum Direction {
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NOCOLL,
};

struct AABB {
	sf::Vector2f min;
	sf::Vector2f max;
};

struct Object {
	// This contains the position and size of the object.
	sf::Rect<float> rect;
	sf::Vector2f pos;
	sf::Vector2f vel;
	AABB aabb;
};

struct Manifold {
	Object *A;
	Object *B;
	float penetration;
	sf::Vector2f normal;
};

// Conversion function
Object valuesToObject(RigidBody const *rigidBody, Transform const *transform,
		      Renderable const *renderable)
{
	sf::Rect<float> rect(transform->position, renderable->size);
	AABB aabb{
	    .min = sf::Vector2f(rect.left, rect.top),
	    .max = sf::Vector2f(rect.left + rect.width, rect.top + rect.height),
	};

	return Object{
	    .rect = rect,
	    .pos = transform->position,
	    .vel = rigidBody->velocity,
	    .aabb = aabb,
	};
}

// Vector math functions
bool AABBvsAABB(AABB a, AABB b)
{
	if (a.max.x < b.min.x || a.min.x > b.max.x) {
		return false;
	}

	if (a.max.y < b.min.y || a.min.y > b.max.y) {
		return false;
	}

	return true;
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

// Collision functions
bool AABBvsAABB(Manifold *m)
{
	Object *A = m->A;
	Object *B = m->B;

	sf::Vector2f n = B->pos - A->pos;

	AABB abox = A->aabb;
	AABB bbox = B->aabb;

	float a_extent = (abox.max.x - abox.min.x) / 2;
	float b_extent = (bbox.max.x - bbox.min.x) / 2;

	float x_overlap = a_extent + b_extent - std::abs(n.x);

	if (x_overlap > 0) {
		a_extent = (abox.max.y - abox.min.y) / 2;
		b_extent = (bbox.max.y - bbox.min.y) / 2;

		float y_overlap = a_extent + b_extent - std::abs(n.y);

		if (y_overlap > 0) {
			if (x_overlap > y_overlap) {
				if (n.x < 0) {
					m->normal = sf::Vector2f(-1.f, 0.f);
				} else {
					m->normal = sf::Vector2f(0.f, 0.f);
				}

				m->penetration = x_overlap;
				return true;
			} else {
				if (n.y < 0) {
					m->normal = sf::Vector2f(0, -1);
				} else {
					m->normal = sf::Vector2f(0, 1);
				}

				m->penetration = y_overlap;
				return true;
			}
		}
	}

	return false;
}

Direction collDirCheck(Manifold *m)
{
	Object *A = m->A;
	Object *B = m->B;

	sf::Vector2f center(A->rect.left + (A->rect.width / 2), A->rect.top + (A->rect.height / 2));
	sf::Vector2f aabb_half_extents(B->rect.width / 2, B->rect.height / 2);
	sf::Vector2f aabb_center(B->rect.left + aabb_half_extents.x,
				 B->rect.top + aabb_half_extents.y);

	sf::Vector2f difference = center - aabb_center;
	sf::Vector2f clamped = clamp(difference, -aabb_half_extents, aabb_half_extents);
	sf::Vector2f closest = aabb_center + clamped;
	difference = closest - center;

	if (vecLength(difference) <= A->rect.width / 2) {
		return vectorDirection(difference);
	}

	return Direction::NOCOLL;
}

sf::Rect<float> basicCollCheck(Manifold *m)
{
	sf::Rect<float> intersection;
	m->A->rect.intersects(m->B->rect, intersection);

	return intersection;
}
} // namespace physics

#endif
