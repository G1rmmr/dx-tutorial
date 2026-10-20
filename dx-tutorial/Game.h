#pragma once

#include <vector>
#include <memory>

#include "Physics.h"
#include "Object.h"
#include "Window.h"
#include "Input.h"
#include "Renderer.h"
#include "SkyBox.h"
#include "UI.h"

namespace dx
{
    class Player;

    class Game
    {
    public:
        Game(Window& window);
        ~Game();
        void Run();

    private:
        Window& mWindow;
        Renderer mRenderer;
        // Bodies are destroyed before the physics service and GPU device.
        Physics mPhysics;
        std::vector<std::unique_ptr<Object>> mObjects;
        Player* mPlayer = nullptr;
        std::unique_ptr<SkyBox> mSkyBox;
        UI mUI;
        Input mInput;

        ULONGLONG mTicksCount;

        int mScore;

        bool mIsRunning;

        void processInput(const float deltaTime);
        void update(const float deltaTime);
        void render();
        void resize(UINT width, UINT height);
        void centerMouse();
    };
}
