#include "Physics.h"
#include <algorithm> // min, max
#include <float.h> // FLT_MAX
#include <iostream>

bool RayPlane(const glm::vec2& rayOrigin, const glm::vec2& rayDir, const glm::vec2& planeNormal, const glm::vec2& pointOnPlane, Hit& hit)
{
    // w
    glm::vec2 w = pointOnPlane - rayOrigin;

    // k = wp / vp simplifies to...
    float k = glm::dot(w, planeNormal) / glm::dot(rayDir, planeNormal);
    
    // intersection point
    hit.contactPoint = rayOrigin + (rayDir * k);
    hit.fraction = k;

    return k >= 0.0f && k <= 1.0f;
}

bool RayBox(const glm::vec2& rayOrigin, const glm::vec2& rayDir, const glm::vec2& boxCenter, const glm::vec2& boxHalfSize, Hit& hit)
{
    glm::vec2 boxMin = boxCenter - boxHalfSize;
    glm::vec2 boxMax = boxCenter + boxHalfSize;

    float low = -FLT_MAX;
    float high = FLT_MAX;

    if (rayDir.x != 0.0f) {
        float t1 = (boxMin.x - rayOrigin.x) / rayDir.x;
        float t2 = (boxMax.x - rayOrigin.x) / rayDir.x;

        low = std::max(low, std::min(t1, t2));
        high = std::min(high, std::max(t1, t2));
    } else if (rayOrigin.x <= boxMin.x || rayOrigin.x >= boxMax.x) {
        return false;
    }

    if (rayDir.y != 0.0f) {
        float t1 = (boxMin.y - rayOrigin.y) / rayDir.y;
        float t2 = (boxMax.y - rayOrigin.y) / rayDir.y;

        low = std::max(low, std::min(t1, t2));
        high = std::min(high, std::max(t1, t2));
    } else if (rayOrigin.y <= boxMin.y || rayOrigin.y >= boxMax.y) {
        return false;
    }

    if (high > low && high > 0.0f && low < 1.0f) {
        hit.contactPoint = rayOrigin + rayDir * low;
        hit.fraction = low;

        float dx = hit.contactPoint.x - boxCenter.x;
        float dy = hit.contactPoint.y - boxCenter.y;
        float px = boxHalfSize.x - abs(dx);
        float py = boxHalfSize.y - abs(dy);

        if (px < py) {
            hit.normal.x = dx > 0 ? 1.0f : -1.0f;  
        } else {
            hit.normal.y = dy > 0 ? 1.0f : -1.0f;  
        }

        return true;
    }

    return false;
}

bool RayCircle(const glm::vec2& rayOrigin, const glm::vec2& rayDir, const glm::vec2& circlePos, const float circleRadius, Hit& hit)
{
    // rather badly written but i mayhaps dont care
    // pretty sure if i actually looked at this code for more than 10 minutes i could remove some of these divisions and square roots
    glm::vec2 originToCirclePos = rayOrigin - circlePos;
    float rayLen = glm::length(rayDir);
    float originToCirclePosLen = glm::length(originToCirclePos);

    float a = rayLen * rayLen;
    float b = 2 * glm::dot(rayDir, originToCirclePos);
    float c = originToCirclePosLen * originToCirclePosLen - circleRadius * circleRadius;

    float q = b * b - 4 * a * c;
    
    if (q < 0) return false;

    float g = 1 / (2 * a);
    q = g * sqrt(q);
    b = (-b * g);
    
    float k = (b - q); // first intersection time

    if (k <= 0 || k >= 1) return false;

    hit.contactPoint = rayOrigin + (rayDir * k);
    hit.depth = glm::length(rayDir * (1 - k));
    hit.normal = glm::normalize(hit.contactPoint - circlePos);
    
    return true;
}
