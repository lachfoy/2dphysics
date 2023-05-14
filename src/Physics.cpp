#include "Physics.h"
// #include <algorithm> // min, max
// #include <float.h> // FLT_MAX
// #include <iostream>

bool CircleVsCircle(const glm::vec2& position_A, float radius_A, const glm::vec2& position_B, float radius_B, CollisionPoints& collision_points)
{
    glm::vec2 ab = position_B - position_A;
    float dist = glm::length(ab);

    if (dist < radius_A + radius_B)
    {
        //float depth = abs(dist - radius_A + radius_B);
        glm::vec2 ab_norm = glm::normalize(ab);
        collision_points.a = position_A + ab_norm * radius_A; // point of contact on A
        collision_points.b = position_B - ab_norm * radius_B; // point of contact on B
        collision_points.normal = ab_norm;
        return true;
    }

    return false;
}
