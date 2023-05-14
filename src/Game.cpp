#include "Game.h"

#include <cstdio>
#include <iostream>
#include <cmath> // floor

#include <glm/gtx/matrix_transform_2d.hpp>

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
    renderer_.Init(960, 640);
    text_renderer_.Init(960, 640);
    font_.Load("fonts/arial_16px.fnt");


    // mFriction = 10.0f;

    // mObjects.push_back(new Object(glm::vec2{ 400, 300 }));
    // mObjects.push_back(new Object(glm::vec2{ 500, 400 }));

    // // pretend velocity vector
    // mObjects[1]->velocity = glm::vec2{ -100.0f, -50.0f };

    // physics_bodies_.push_back(std::make_unique<PhysicsBody>(glm::vec2{ 400.0f, 300.0f }, 50.0f));
    // physics_bodies_.push_back(std::make_unique<PhysicsBody>(glm::vec2{ 540.0f, 330.0f }, 50.0f));
}

void Game::HandleInput()
{
    if (input_.IsKeyPressed(SDL_SCANCODE_SPACE)) physics_bodies_.push_back(std::make_unique<PhysicsBody>(glm::vec2{ mouse_position_[0], mouse_position_[1] }, 50.0f));
}

void Game::Update(float delta_time)
{
    // integrate
    for (const auto& physics_body : physics_bodies_) {
        glm::vec2 acceleration = physics_body->force / physics_body->mass;
        acceleration -= physics_body->velocity * physics_body->friction; // apply friction

        physics_body->velocity += acceleration * delta_time;
        physics_body->position += physics_body->velocity * delta_time;
    }
    
    // constraint = pointB - pointA dot normal >= 0
    for (const auto& physics_body_A : physics_bodies_)
    {
        for (const auto& physics_body_B : physics_bodies_)
        {
            if (physics_body_A == physics_body_B) break;

            CollisionPoints collision_points;
            if (CircleVsCircle(physics_body_A->position, physics_body_A->radius, physics_body_B->position, physics_body_B->radius, collision_points))
            {
                glm::vec2 j_va = -collision_points.normal;
                glm::vec2 j_vb = collision_points.normal;
                float k = (1.0f / physics_body_A->mass) + (1.0f / physics_body_B->mass); // mass matrix
                float effective_mass = 1.0f / k;

                // bias
                float b = (0.1f / delta_time) * glm::dot(collision_points.b - collision_points.a, collision_points.normal);

                float jv = glm::dot(j_va, physics_body_A->velocity) + glm::dot(j_vb, physics_body_B->velocity);
                float lambda = effective_mass * (-(jv + b));

                // apply impulse
                physics_body_A->velocity += (1.0f / physics_body_A->mass) * j_va * lambda;
                physics_body_B->velocity += (1.0f / physics_body_B->mass) * j_vb * lambda;
            }
        }
        
    }
}

void Game::Draw()
{
    // glm::vec2 circleAPos = { 400, 300 };
    // CollisionPoints collisionPoints;
    // bool collision = CircleVsCircle(circleAPos, 60.0f, glm::vec2{ mouse_position_[0], mouse_position_[1] }, 80.0f, collisionPoints);

    // renderer_.DrawCircle(circleAPos, 60.0f, glm::vec3{ 1, 0, 1 });
    // renderer_.DrawCircle(glm::vec2{ mouse_position_[0], mouse_position_[1] }, 80.0f, collision ? glm::vec3{ 1, 0, 0 } : glm::vec3{ 0, 1, 1 });

    // if (collision)
    // {
    //     renderer_.DrawCircle(collisionPoints.a, 10.0f, glm::vec3{ 1, 1, 0 });
    //     renderer_.DrawCircle(collisionPoints.b, 10.0f, glm::vec3{ 0, 0, 1 });
    // }

    for (const auto& physics_body : physics_bodies_)
    {
        renderer_.DrawCircle(physics_body->position, physics_body->radius, glm::vec3{ 1, 1, 1 });
    }
}

void Game::Destroy()
{
    SDL_GL_DeleteContext(context_);
    SDL_DestroyWindow(window_);
    
    SDL_Quit();
}