#ifndef GRID_ENTITY_H_
#define GRID_ENTITY_H_

#include <glm/glm.hpp>

class GridEntity
{
public:
    ~GridEntity() = default;

private:
    glm::vec2 position;
    glm::vec2 velocity; // rate of change of position over time
    
    // acceleration is the rate of change of velocity over time.
    
    glm::vec2 force; // idk if we will use this
    float radius;
    float mass;
    float friction;
    
};

#endif