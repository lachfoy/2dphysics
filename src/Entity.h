#ifndef ENTITY_H_
#define ENTITY_H_

#include <glm/glm.hpp>

class Entity
{
public:
    Entity() = default;
    Entity(const glm::vec2& position);
    ~Entity() = default;

    glm::vec2 const& Position() const { return position_; }
    glm::vec2 const& Velocity() const { return velocity_; }
    float Radius() const { return radius_; }
    float Mass() const { return mass_; }
    float Friction() const { return friction_; }

    void AddVelocity(const glm::vec2& velocity);
    void Update(float delta_time);

protected:
    glm::vec2 position_;
    glm::vec2 velocity_; // rate of change of position over time
    
    // acceleration is the rate of change of velocity over time.
    
    glm::vec2 force; // idk if we will use this
    float radius_;
    float mass_;
    float friction_;

};

#endif