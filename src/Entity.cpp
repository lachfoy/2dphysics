#include "Entity.h"

Entity::Entity(const glm::vec2& position)
    : position_(position)
{
    // default stuff, remove later
    radius_ = 10.0f;
    mass_ = 5.0f;
    friction_ = 1.0f;
}

void Entity::AddVelocity(const glm::vec2& velocity)
{
    velocity_ += velocity;
}

void Entity::Update(float delta_time)
{
    // physics integration
    glm::vec2 addedVelocity = glm::vec2(0.0f);    
    glm::vec2 acceleration = (velocity_ + addedVelocity - velocity_) / delta_time;

    //glm::vec2 acceleration = entity->force / entity->mass;
    acceleration -= velocity_ * friction_; // apply friction

    velocity_ += acceleration * delta_time;
    position_ += 0.5f * (velocity_ + (velocity_ + acceleration * delta_time)) * delta_time;
}