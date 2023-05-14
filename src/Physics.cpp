#include "Physics.h"
#include <algorithm> // min, max
#include <float.h> // FLT_MAX
#include <iostream>

bool CircleVsCircle(const glm::vec2& posA, float radiusA, const glm::vec2& posB, float radiusB, CollisionPoints& collisionPoints)
{
    glm::vec2 ab = posB - posA;
    float dist = glm::length(ab);

    if (dist < radiusA + radiusB)
    {
        //float depth = abs(dist - radiusA + radiusB);
        glm::vec2 abNorm = glm::normalize(ab);
        collisionPoints.a = posA + abNorm * radiusA; // point of contact on A
        collisionPoints.b = posB - abNorm * radiusB; // point of contact on B
        collisionPoints.normal = abNorm;
        return true;
    }

    return false;
}
