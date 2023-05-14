#ifndef GAME_H_
#define GAME_H_

#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <SDL2/SDL_opengl.h>

#include "Input.h"
#include "Renderer.h"
#include "TextRenderer.h"
#include "Font.h"

#include "Physics.h"

#include <vector>
#include <memory>

class Game
{
public:
    Game() {};

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

    std::vector<std::unique_ptr<PhysicsBody>> physics_bodies_;

};

#endif