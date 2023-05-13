#include "Game.h"

#include <cstdio>
#include <iostream>
#include <cmath> // floor

#include "Colors.h"
#include <glm/gtx/matrix_transform_2d.hpp>

bool Game::Init()
{
    // initialize SDL video
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL video could not be initialized: %s\n", SDL_GetError());
        return false;
    }

    // create main window
    mWindow = SDL_CreateWindow(
        "test",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        960,
        640,
        SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
    );
    if (!mWindow) {
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

    mContext = SDL_GL_CreateContext(mWindow);
    if (!mContext) {
        printf("GL Context could not be created: %s\n", SDL_GetError());
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
        return false;
    }

    // initialize GLAD
    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        printf("GLAD could not be loaded\n");
        SDL_GL_DeleteContext(mContext);
        SDL_DestroyWindow(mWindow);
        SDL_Quit();
        return false;
    }

    // enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // this functionality exists i guess?
    glEnable(GL_LINE_SMOOTH);
    glLineWidth(1);
    
    return true;
}

void Game::Run()
{
    Create();

    Uint32 lastTime = 0;

    SDL_Event event;
    int fps = 0;
    int fpsCount = 0;
    float fpsInterval = 0.0f;

    // main loop
    while (mRunning) {
        // handle input events
        while (SDL_PollEvent(&event) != 0) {
            switch(event.type) {
            case SDL_QUIT:
                mRunning = false;
                break;
            case SDL_KEYDOWN:
                mInput.SetKeyDown(event.key.keysym.scancode);
                break;
            case SDL_KEYUP:
                mInput.SetKeyUp(event.key.keysym.scancode);
                break;
            case SDL_MOUSEMOTION:
                SDL_GetMouseState(&mMousePosition[0], &mMousePosition[1]);
            }
        }

        HandleInput();

        // update input
        mInput.UpdateInput();

        // calculate delta time
        Uint32 currentTime = SDL_GetTicks();
		float deltaTime = (currentTime - lastTime) / 1000.0f;
        lastTime = currentTime;
        
        // calculate fps
        fpsCount++;
        fpsInterval += deltaTime;
        if (fpsInterval > 1.0f) {
            fps = fpsCount;
            fpsCount = 0;
            fpsInterval = 0.0f;
        }

        // update
        Update(deltaTime);

        // clear screen
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw
        Draw();

        mTextRenderer.DrawText("dt: " + std::to_string(deltaTime), 4, 4, mFont, 1.0f);

        // swap buffers
        SDL_GL_SwapWindow(mWindow);
    }

    Destroy();
}

void Game::Create()
{
    mRenderer.Init(960, 640);
    mTextRenderer.Init(960, 640);
    mFont.Load("fonts/arial_16px.fnt");

    mPlayerPos = glm::vec2{ 400.0f, 300.0f };
    mPlayerSize = glm::vec2{ 64.0f, 64.0f };
    mRayPos = glm::vec2{ 100.0f, 100.0f };

    mSpeed = 4000.0f;
    mFriction = 10.0f;

    const int tileMapWidth = 15;
    const int tileMapHeight = 10;
    int tileMap[tileMapWidth * tileMapHeight] = {
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1,
        1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1,
        1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1
    };
    
    mTileSize = glm::vec2{ 64.0f, 64.0f };
    for (int x = 0; x < tileMapWidth; x++) {
        for (int y = 0; y < tileMapHeight; y++) {
            if (tileMap[x + tileMapWidth * y] == 1) {
                mTileCenters.push_back(glm::vec2{ x * mTileSize.x, y * mTileSize.y } + mTileSize * 0.5f);
            }
        }
    }

    mObjects.push_back(new Object(glm::vec2{ 400, 300 }));
    mObjects.push_back(new Object(glm::vec2{ 500, 400 }));

    // pretend velocity vector
    mObjects[1]->velocity = glm::vec2{ -100.0f, -50.0f };
}

void Game::HandleInput()
{
    if (mInput.IsKeyPressed(SDL_SCANCODE_ESCAPE)) mRunning = false; //esc quit
    if (mInput.IsKeyHeld(SDL_SCANCODE_W)) mDir.y = -1.0f;
    if (mInput.IsKeyHeld(SDL_SCANCODE_A)) mDir.x = -1.0f;
    if (mInput.IsKeyHeld(SDL_SCANCODE_S)) mDir.y = 1.0f;
    if (mInput.IsKeyHeld(SDL_SCANCODE_D)) mDir.x = 1.0f;
}

void Game::Update(float deltaTime)
{
    //mObjects[1]->position = glm::vec2{ mMousePosition[0], mMousePosition[1] };
    mObjects[1]->force = mDir * mSpeed * deltaTime;

    for (const auto& object : mObjects) {
        //object->force = glm::vec2{ 0.0f, 9000.0f }; // some kind of force
        glm::vec2 acceleration = object->force / object->mass;
        acceleration -= object->velocity * 1.0f; // apply friction

        // integrate
        object->velocity += acceleration * deltaTime;
        glm::vec2 dp = object->velocity * deltaTime;

        // test collision
        Hit hit;
        for (const auto& other : mObjects) {
            if (object == other) break;

            bool collision = RayCircle(object->position, dp, other->position, object->radius + other->radius, hit);
            if (collision) {
                // ahh
                object->position = hit.contactPoint;
                dp = dp - 1 * glm::(dp, hit.normal) * hit.normal;
            }

            std::cout << (collision ? "true" : "false") <<"\n";
        }



        object->position += dp;
    }


    mDir = {};
}

void Game::Draw()
{
    for (const auto& object : mObjects) {
        mRenderer.DrawCircle(object->position, object->radius, glm::vec3{ 0.0f, 1.0f, 0.0f });
        //mRenderer.DrawLine(object->position, object->position + object->velocity, glm::vec3{ 0.0f, 1.0f, 1.0f });
    }

    // mRenderer.DrawCircle(mObjects[0]->position, mObjects[0]->radius + mObjects[1]->radius, glm::vec3{ 1.0f, 0.0f, 1.0f });
    // mRenderer.DrawCircle(mObjects[1]->position + mObjects[1]->velocity, mObjects[1]->radius, glm::vec3{ 1.0f, 1.0f, 0.0f });

    // // the depth should be 
    // mRenderer.DrawLine(mHit.contactPoint, mHit.contactPoint + mHit.normal * mHit.depth, glm::vec3{ 1.0f, 0.0f, 0.0f });
    //mHit
}

void Game::Destroy()
{

    for (const auto& object : mObjects) {
        delete object;
    }

    SDL_GL_DeleteContext(mContext);
    SDL_DestroyWindow(mWindow);
    
    SDL_Quit();
}