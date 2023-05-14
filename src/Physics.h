#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <glm/glm.hpp>

// hit info
struct CollisionPoints
{
    glm::vec2 a;
    glm::vec2 b;
    float depth;
    glm::vec2 normal;
};

bool CircleVsCircle(const glm::vec2& posA, float radiusA, const glm::vec2& posB, float radiusB, CollisionPoints& collisionPoints);

#endif // PHYSICS_H_