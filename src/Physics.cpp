#include "Physics.h"
#include <cmath> // sqrt
//#include <algorithm> // min, max
//#include <float.h> // FLT_MAX
// #include <iostream>

bool CircleVsCircle(const glm::vec2& position_A, float radius_A, const glm::vec2& position_B, float radius_B, CollisionPoints& collision_points)
{
    glm::vec2 ab = position_B - position_A;

    float distSq = glm::dot(ab, ab);
    float radiusSum = radius_A + radius_B;
    float radiusSumSq = radiusSum * radiusSum;

    if (distSq < radiusSumSq)
    {
        float dist = std::sqrt(distSq);
        glm::vec2 ab_norm = ab / dist;
        collision_points.a = position_A + ab_norm * radius_A; // point of contact on A
        collision_points.b = position_B - ab_norm * radius_B; // point of contact on B
        collision_points.normal = ab_norm;
        return true;
    }

    return false;
}


bool CircleVsAABB(const glm::vec2& circleCenter, float circleRadius, const glm::vec2& aabbMin, const glm::vec2& aabbMax, CollisionPoints& collisionPoints)
{
    // circle = a
    // box = b

    glm::vec2 closestPoint = glm::clamp(circleCenter, aabbMin, aabbMax);
    glm::vec2 circleToClosest = closestPoint - circleCenter;
    float distanceSq = glm::dot(circleToClosest, circleToClosest);

    if (distanceSq <= (circleRadius * circleRadius))
    {
        float distance = std::sqrt(distanceSq);

        if (distance > 0.0f)
        {
            collisionPoints.normal = circleToClosest / distance;
            collisionPoints.a = circleCenter + collisionPoints.normal * circleRadius;
            collisionPoints.b = closestPoint - collisionPoints.normal * circleRadius;
        }
        else
        {
            // Special case when the circle center is inside the AABB
            collisionPoints.normal = glm::vec2(0.0f, 0.0f);
            collisionPoints.a = circleCenter;
            collisionPoints.b = closestPoint;
        }

        return true;
    }

    return false;
}

// bool RayBox(const glm::vec2& rayOrigin, const glm::vec2& rayDir, const glm::vec2& boxMin, const glm::vec2& boxMax, CollisionPoints& collision_points)
// {
//     glm::vec2 boxCenter = 0.5f * (boxMin + boxMax);
//     glm::vec2 boxHalfSize = 0.5f * (boxMax - boxMin);

//     float low = -FLT_MAX;
//     float high = FLT_MAX;

//     if (rayDir.x != 0.0f) {
//         float t1 = (boxMin.x - rayOrigin.x) / rayDir.x;
//         float t2 = (boxMax.x - rayOrigin.x) / rayDir.x;

//         low = std::max(low, std::min(t1, t2));
//         high = std::min(high, std::max(t1, t2));
//     } else if (rayOrigin.x <= boxMin.x || rayOrigin.x >= boxMax.x) {
//         return false;
//     }

//     if (rayDir.y != 0.0f) {
//         float t1 = (boxMin.y - rayOrigin.y) / rayDir.y;
//         float t2 = (boxMax.y - rayOrigin.y) / rayDir.y;

//         low = std::max(low, std::min(t1, t2));
//         high = std::min(high, std::max(t1, t2));
//     } else if (rayOrigin.y <= boxMin.y || rayOrigin.y >= boxMax.y) {
//         return false;
//     }

//     // if (high > low && high > 0.0f && low < 1.0f) {
//     //     hit.contactPoint = rayOrigin + rayDir * low;
//     //     hit.fraction = low;

//     //     float dx = hit.contactPoint.x - boxCenter.x;
//     //     float dy = hit.contactPoint.y - boxCenter.y;
//     //     float px = boxHalfSize.x - abs(dx);
//     //     float py = boxHalfSize.y - abs(dy);

//     //     if (px < py) {
//     //         hit.normal.x = dx > 0 ? 1.0f : -1.0f;  
//     //     } else {
//     //         hit.normal.y = dy > 0 ? 1.0f : -1.0f;  
//     //     }

//     //     return true;
//     // }

//     return false;
// }
