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

struct Object
{
    Object(glm::vec2 a_position) {
        position = a_position;
        radius = 100.0f;
        mass = 5.0f;
        velocity = {};
        force = {};
    }

    glm::vec2 position;
    glm::vec2 velocity; // rate of change of position over time
    
    // acceleration is the rate of change of velocity over time.
    
    glm::vec2 force;
    float radius;
    float mass;
};

class Game
{
public:
    Game() {};

    bool Init();
    void Run();

private:
    void Create();
    void HandleInput();
    void Update(float deltaTime);
    void Draw();
    void Destroy();
    
    SDL_Window* mWindow;
    SDL_GLContext mContext;

    bool mRunning = true;

    Input mInput;
    Renderer mRenderer;
    TextRenderer mTextRenderer;
    Font mFont;


    int mMousePosition[2];
    glm::vec2 mDir;
    float mSpeed;

    // glm::vec2 mAcceleration;
    // glm::vec2 mVelocity;
    // float mFriction;

    // std::vector<Object*> mObjects;

};

#endif