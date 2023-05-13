#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <glm/glm.hpp>

// hit info
struct Hit
{
    glm::vec2 contactPoint;
    float fraction;
    glm::vec2 normal;
    float depth;
};

bool RayPlane(const glm::vec2& rayOrigin, const glm::vec2& rayDir, const glm::vec2& planeNormal, const glm::vec2& pointOnPlane, Hit& hit);
bool RayBox(const glm::vec2& rayOrigin, const glm::vec2& rayDir, const glm::vec2& boxCenter, const glm::vec2& boxHalfSize, Hit& hit);
bool RayCircle(const glm::vec2& rayOrigin, const glm::vec2& rayDir, const glm::vec2& circlePos, const float circleRadius, Hit& hit);

#endif // PHYSICS_H_