#include "EntityPlayer.h"

EntityPlayer::EntityPlayer()
{
    radius_ = 10.0f;
    mass_ = 5.0f;
    friction_ = 1.0f;
}

EntityPlayer::EntityPlayer(const glm::vec2 position)
    : Entity(position)
{
    radius_ = 10.0f;
    mass_ = 5.0f;
    friction_ = 1.0f;
}

