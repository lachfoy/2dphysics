#include "Game.h"

#include <cstdio>
#include <iostream>
#include <cmath> // floor

#include <glm/gtx/matrix_transform_2d.hpp>

Game::Game()
{
}

bool Game::Init()
{
    // initialize SDL video
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL video could not be initialized: %s\n", SDL_GetError());
        return false;
    }

    // create main window
    window_ = SDL_CreateWindow(
        "test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        960,
        640,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    if (!window_) {
        printf("SDL window could not be created: %s\n", SDL_GetError());
        SDL_Quit();
        return false;
    }

    // initialize renderering context
    //Use OpenGL 3.3 core // should be ok for anything about a decade old?
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

    // double buffering
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    context_ = SDL_GL_CreateContext(window_);
    if (!context_) {
        printf("GL Context could not be created: %s\n", SDL_GetError());
        SDL_DestroyWindow(window_);
        SDL_Quit();
        return false;
    }

    // initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("GLAD could not be loaded\n");
        SDL_GL_DeleteContext(context_);
        SDL_DestroyWindow(window_);
        SDL_Quit();
        return false;
    }

    // enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // this functionality exists i guess?
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(2);
    
    return true;
}

void Game::Run()
{
    Create();

    Uint32 last_time = 0;

    SDL_Event event;
    int fps = 0;
    int fps_count = 0;
    float fps_interval = 0.0f;

    // main loop
    bool running;
    while (running) {
        // handle input events
        while (SDL_PollEvent(&event) != 0) {
            switch(event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
                input_.SetKeyDown(event.key.keysym.scancode);
                break;
            case SDL_KEYUP:
                input_.SetKeyUp(event.key.keysym.scancode);
                break;
            case SDL_MOUSEMOTION:
                SDL_GetMouseState(&mouse_position_[0], &mouse_position_[1]);
            }
        }

        HandleInput();

        // update input
        input_.UpdateInput();

        // calculate delta time
        Uint32 current_time = SDL_GetTicks();
		float delta_time = (current_time - last_time) / 1000.0f;
        last_time = current_time;
        
        // calculate fps
        fps_count++;
        fps_interval += delta_time;
        if (fps_interval > 1.0f) {
            fps = fps_count;
            fps_count = 0;
            fps_interval = 0.0f;
        }

        // update
        Update(delta_time);

        // clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw
        Draw();

        text_renderer_.DrawText("dt: " + std::to_string(delta_time), 4, 4, font_, 1.0f);

        // swap buffers
        SDL_GL_SwapWindow(window_);
    }

    Destroy();
}

void Game::Create()
{
    renderer_.Init(960 / 4, 640 / 4);
    text_renderer_.Init(960 / 2, 640 / 2);
    font_.Load("fonts/arial_16px.fnt");

    // Player!!
    player_ = std::make_shared<EntityPlayer>(glm::vec2(400.0f / 4, 300.0f /4));
    entity_list_.push_back(player_);

}

void Game::HandleInput()
{
    if (input_.IsKeyPressed(SDL_SCANCODE_SPACE))
    {
        entity_list_.push_back(std::make_unique<Entity>(glm::vec2{ mouse_position_[0] / 4, mouse_position_[1] /4 }));
    }
    if (input_.IsKeyHeld(SDL_SCANCODE_W))
    {
        dir_.y = -1.0f;
        
    }
    if (input_.IsKeyHeld(SDL_SCANCODE_A)) dir_.x = -1.0f;
    if (input_.IsKeyHeld(SDL_SCANCODE_S)) dir_.y = 1.0f;
    if (input_.IsKeyHeld(SDL_SCANCODE_D)) dir_.x = 1.0f;
}

void Game::Update(float delta_time)
{
    player_->AddVelocity(dir_ * 2.0f);
    
    /// PHYSICS
    for (const auto& entity : entity_list_) {
        // INTEGRATE
        entity->Update(delta_time);

        // SOLVE COLLISION
        for (const auto& other : entity_list_)
        {
            if (entity == other) break;

            CollisionPoints collision_points;
            if (CircleVsCircle(entity->Position(), entity->Radius(), other->Position(), other->Radius(), collision_points))
            {
                glm::vec2 j_va = -collision_points.normal;
                glm::vec2 j_vb = collision_points.normal;
                float k = (1.0f / entity->Mass()) + (1.0f / other->Mass()); // mass matrix
                float effective_mass = 1.0f / k;

                // bias
                float b = (0.1f / delta_time) * glm::dot(collision_points.b - collision_points.a, collision_points.normal);

                float jv = glm::dot(j_va, entity->Velocity()) + glm::dot(j_vb, other->Velocity());
                float lambda = effective_mass * (-(jv + b));

                // apply impulse
                entity->AddVelocity((1.0f / entity->Mass()) * j_va * lambda);
                other->AddVelocity((1.0f / other->Mass()) * j_vb * lambda);
            }
        }
    }

    dir_ = {};//reset dir
}

void Game::Draw()
{
    for (const auto& physics_body : entity_list_)
    {
        renderer_.DrawCircle(physics_body->Position(), physics_body->Radius(), glm::vec3{ 1, 1, 1 });
    }
}

void Game::Destroy()
{
    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window_);
    
    SDL_Quit();
}