#ifndef GAME_H_
#define GAME_H_

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <SDL2/SDL_opengl.h>

#include <vector>
#include <memory>

#include "Input.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "Font.h"

#include "Entity.h"
#include "EntityPlayer.h"
#include "Physics.h"

typedef std::shared_ptr<Entity> EntityPtr;
typedef std::vector<EntityPtr> EntityList;

class Game
{
public:
    Game();

    bool Init();
    void Run();

private:
    void Create();
    void HandleInput();
    void Update(float delta_time);
    void Draw();
    void Destroy();
    
    SDL_Window* window_;
    SDL_GLContext context_;

    Input input_;
    Renderer renderer_;
    TextRenderer text_renderer_;
    Font font_;


    int mouse_position_[2];
    glm::vec2 dir_;

    EntityPtr player_;
    EntityList entity_list_;

};

#endif