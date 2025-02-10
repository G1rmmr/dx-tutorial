#include <iostream>

#include "Game.h"

#include <bullet/btBulletCollisionCommon.h>
#include <bullet/btBulletDynamicsCommon.h>

const int SCREEN_WIDTH = 1280;
const int SCREEN_HEIGHT = 720;

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow)
{
    /*
    core::Game game;

    if(!game.Initialize(hInstance, SCREEN_WIDTH, SCREEN_HEIGHT, nCmdShow))
    {
        std::cerr << "GAME INITIALIZATION FAILED!!\n";
        return -1;
    }

    ShowCursor(FALSE);

    game.Run();
    game.Cleanup();
    */

    btBroadphaseInterface* broadphase = new btDbvtBroadphase();
    btDefaultCollisionConfiguration* collisionConfig = new btDefaultCollisionConfiguration();
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfig);
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver();
    btDiscreteDynamicsWorld* world = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfig);
    world->setGravity(btVector3(0, -9.81f, 0));

    world->stepSimulation(1.f / 60.f, 10);

    MessageBox(nullptr, L"Bullet test OK!", L"Error", MB_OK);

    delete world;
    delete solver;
    delete dispatcher;
    delete collisionConfig;
    delete broadphase;

    return 0;
}
