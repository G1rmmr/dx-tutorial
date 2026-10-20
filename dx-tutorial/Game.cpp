#include "Game.h"

#include "Player.h"
#include "Enemy.h"
#include "Floor.h"
#include "Mesh.h"
#include "ModelAsset.h"
#include "Rigidbody.h"
#include "Shader.h"
#include "Transform.h"

#include <algorithm>
#include <filesystem>
#include <numeric>
#include <span>
#include <stdexcept>
#include <type_traits>

using namespace DirectX;
using namespace dx;

namespace
{
    void addDrawable(Object& object, ID3D11Device* device, const std::vector<Vertex>& vertices,
        const Shader& shader)
    {
        std::vector<std::uint32_t> indices(vertices.size());
        std::iota(indices.begin(), indices.end(), 0U);
        object.EmplaceComponent<Mesh>(device, std::span<const Vertex>(vertices),
            std::span<const std::uint32_t>(indices));
        object.EmplaceComponent<Shader>(shader);
    }

    void addDrawable(Object& object, ID3D11Device* device, const ModelAsset& model,
        const Shader& shader)
    {
        object.EmplaceComponent<Mesh>(device, std::span<const Vertex>(model.Vertices),
            std::span<const std::uint32_t>(model.Indices));
        object.EmplaceComponent<Shader>(shader);
    }

    class ModelProp final : public Object
    {
    public:
        explicit ModelProp(const XMFLOAT3& position)
        {
            IsStatic = true;
            EmplaceComponent<Transform>().Position = position;
        }

        void Update(float) override {}
    };
}

Game::Game(Window& window)
    : mWindow(window)
    , mRenderer(window.GetHandle(), window.GetWidth(), window.GetHeight())
    , mTicksCount(GetTickCount64())
    , mScore(0)
    , mIsRunning(true)
{
    const Shader shader(mRenderer.GetDevice(), L"VertexShader.hlsl", "VSMain",
        L"PixelShader.hlsl", "PSMain");

    auto floor = std::make_unique<Floor>(XMFLOAT3(0.f, -0.5f, 0.f),
        XMFLOAT3(20.f, 0.5f, 20.f), XMFLOAT4(0.35f, 0.4f, 0.35f, 1.f));

    addDrawable(*floor, mRenderer.GetDevice(), floor->GetVertices(), shader);
    mPhysics.AddRigidBody(*floor->GetComponent<Rigidbody>());
    mObjects.emplace_back(std::move(floor));

    // Import once at startup, keeping the existing Object/Mesh rendering path.
    ModelImportOptions soldierOptions;
    soldierOptions.Size = 2.4f;
    soldierOptions.Origin = ModelOrigin::Center;
    const auto addSoldier = [&](const wchar_t* filename, float x, float z)
    {
        const ModelAsset model = LoadFbxModel(std::filesystem::path(L"assets/models") / filename,
            soldierOptions);
        const XMFLOAT3 halfExtents((model.Max.x - model.Min.x) * 0.5f,
            (model.Max.y - model.Min.y) * 0.5f, (model.Max.z - model.Min.z) * 0.5f);
        auto enemy = std::make_unique<Enemy>(XMFLOAT3(x, halfExtents.y, z), halfExtents);
        addDrawable(*enemy, mRenderer.GetDevice(), model, shader);
        mPhysics.AddRigidBody(*enemy->GetComponent<Rigidbody>());
        mObjects.emplace_back(std::move(enemy));
    };
    addSoldier(L"enemy.fbx", -2.f, 8.f);
    addSoldier(L"elite.fbx", 2.f, 10.f);

    // Display the weapon assets as scenery; gameplay remains in Player.
    const auto addProp = [&](const wchar_t* filename, float size, const XMFLOAT3& position)
    {
        ModelImportOptions options;
        options.Size = size;
        options.SizeAxis = ModelSizeAxis::Longest;
        const ModelAsset model = LoadFbxModel(std::filesystem::path(L"assets/models") / filename,
            options);
        auto prop = std::make_unique<ModelProp>(position);
        addDrawable(*prop, mRenderer.GetDevice(), model, shader);
        mObjects.emplace_back(std::move(prop));
    };
    addProp(L"pistol.fbx", 0.8f, XMFLOAT3(-2.f, 0.f, 4.f));
    addProp(L"machine-gun.fbx", 1.4f, XMFLOAT3(0.f, 0.f, 4.f));
    addProp(L"grenades.fbx", 0.6f, XMFLOAT3(2.f, 0.f, 4.f));

    auto player = std::make_unique<Player>();
    mPlayer = player.get();
    mPhysics.AddRigidBody(*player->GetComponent<Rigidbody>());
    mObjects.emplace_back(std::move(player));
    mPlayer->GetCamera().SetPerspective(XMConvertToRadians(60.f),
        static_cast<float>(window.GetWidth()) / window.GetHeight(), 0.1f, 100.f);

    mSkyBox = std::make_unique<SkyBox>(mRenderer.GetDevice(), mRenderer.GetContext());
    if (!mUI.Initialize(&mRenderer, static_cast<int>(window.GetWidth()),
        static_cast<int>(window.GetHeight())))
    {
        throw std::runtime_error("UI initialization failed");
    }
    mTicksCount = GetTickCount64();
}

Game::~Game()
{
    mWindow.SetMouseLook(false);
    mPhysics.ClearBodies();
}

void Game::Run()
{
    while (mIsRunning)
    {
        mInput.BeginFrame();
        UINT resizeWidth = 0;
        UINT resizeHeight = 0;
        Event event;
        while (mWindow.PollEvent(event))
        {
            mInput.HandleEvent(event);
            std::visit([&](const auto& value)
            {
                using T = std::decay_t<decltype(value)>;
                if constexpr (std::is_same_v<T, QuitEvent> || std::is_same_v<T, CloseEvent>)
                {
                    mIsRunning = false;
                }
                else if constexpr (std::is_same_v<T, ResizeEvent>)
                {
                    if (value.Width && value.Height)
                    {
                        resizeWidth = value.Width;
                        resizeHeight = value.Height;
                    }
                }
            }, event);
        }
        if (!mIsRunning)
        {
            break;
        }
        if (resizeWidth && resizeHeight)
        {
            resize(resizeWidth, resizeHeight);
        }

        if (!mWindow.IsActive())
        {
            mWindow.SetMouseLook(false);
            mInput.HandleEvent(FocusLostEvent{});
            mWindow.WaitForEvents();
            mTicksCount = GetTickCount64();
            continue;
        }
        if (mRenderer.IsOccluded() && !mRenderer.TestVisibility())
        {
            mWindow.SetMouseLook(false);
            mInput.HandleEvent(FocusLostEvent{});
            mWindow.WaitForEvents(100);
            mTicksCount = GetTickCount64();
            continue;
        }
        mWindow.SetMouseLook(true);

        const ULONGLONG currentTime = GetTickCount64();
        const float deltaTime = std::min(static_cast<float>(currentTime - mTicksCount) / 1000.f, 0.05f);
        mTicksCount = currentTime;
        processInput(deltaTime);
        if (!mIsRunning)
        {
            break;
        }
        update(deltaTime);
        render();
    }
}

void Game::centerMouse()
{
    int x = 0;
    int y = 0;
    if (mWindow.CenterCursor(x, y))
    {
        // Match the next frame's absolute baseline to the cursor warp.
        mInput.HandleEvent(MouseMoveEvent{x, y});
    }
}

void Game::processInput(const float deltaTime)
{
    if (mInput.IsKeyDown(Key::Escape))
    {
        mIsRunning = false;
        return;
    }
    mPlayer->ProcessMouseMovement(static_cast<float>(mInput.GetMouseDeltaX()),
        static_cast<float>(mInput.GetMouseDeltaY()));
    mPlayer->ProcessKeyboard(&mPhysics.GetDynamics(), deltaTime,
        mInput.IsKeyDown(Key::W), mInput.IsKeyDown(Key::S), mInput.IsKeyDown(Key::A),
        mInput.IsKeyDown(Key::D), mInput.IsKeyDown(Key::Space));
    if (mInput.WasMouseButtonPressed(MouseButton::Left) &&
        mPlayer->Shoot(&mPhysics.GetDynamics()))
    {
        ++mScore;
    }
    centerMouse();
}

void Game::update(const float deltaTime)
{
    mPhysics.Update(deltaTime);
    for (const auto& object : mObjects)
    {
        object->Update(deltaTime);
    }
}

void Game::resize(UINT width, UINT height)
{
    if (width == mRenderer.GetWidth() && height == mRenderer.GetHeight())
    {
        return;
    }
    mUI.ReleaseTarget();
    mRenderer.Resize(width, height);
    if (!mUI.Resize(static_cast<int>(width), static_cast<int>(height)))
    {
        throw std::runtime_error("UI resize failed");
    }
    mPlayer->GetCamera().SetPerspective(XMConvertToRadians(60.f),
        static_cast<float>(width) / height, 0.1f, 100.f);
    mInput.HandleEvent(FocusLostEvent{});
}

void Game::render()
{
    mRenderer.BeginFrame({0.f, 0.f, 0.f, 1.f});
    const Camera& camera = mPlayer->GetCamera();
    mSkyBox->Render(mRenderer.GetContext(), mRenderer.GetMatrixBuffer(),
        camera.GetView(), camera.GetProjection());
    for (const auto& object : mObjects)
    {
        if (object.get() != mPlayer)
        {
            mRenderer.Draw(*object, camera);
        }
    }
    mUI.Begin();
    mUI.Draw(L"Score: " + std::to_wstring(mScore), 50.f, 50.f, 32.f);
    mUI.Draw(L"WASD Move | Mouse Look | Space Jump | Left Click Fire | Esc Exit", 50.f, 96.f, 18.f);
    mUI.End();
    mRenderer.EndFrame();
}
