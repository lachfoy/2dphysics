#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <glm/glm.hpp>

struct PhysicsBody
{
    PhysicsBody(const glm::vec2& position, float radius)
        : position(position), radius(radius)
    {
        force = glm::vec2(0.0f);
        velocity = glm::vec2(0.0f);
        mass = 5.0f;
        friction = 1.0f;
    }

    ~PhysicsBody() = default;

    glm::vec2 position;
    glm::vec2 velocity; // rate of change of position over time
    
    // acceleration is the rate of change of velocity over time.
    
    glm::vec2 force;
    float radius;
    float mass;
    float friction;
};

// hit info
struct CollisionPoints
{
    glm::vec2 a;
    glm::vec2 b;
    glm::vec2 normal;
};

bool CircleVsCircle(const glm::vec2& position_A, float radius_A, const glm::vec2& position_B, float radius_B, CollisionPoints& collision_points);
bool CircleVsAABB(const glm::vec2& circleCenter, float circleRadius, const glm::vec2& aabbMin, const glm::vec2& aabbMax, CollisionPoints& collisionPoints);

//bool RayBox(const glm::vec2& rayOrigin, const glm::vec2& rayDir, const glm::vec2& boxMin, const glm::vec2& boxMax, CollisionPoints& collision_points);

#endif // PHYSICS_H_