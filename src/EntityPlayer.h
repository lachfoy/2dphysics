#ifndef ENTITY_PLAYER_H_
#define ENTITY_PLAYER_H_

#include "Entity.h"

class EntityPlayer : public Entity 
{
public:
    EntityPlayer();
    EntityPlayer(const glm::vec2 position);
    ~EntityPlayer() = default;

};

#endif