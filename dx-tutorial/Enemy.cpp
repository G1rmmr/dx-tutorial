#include "Enemy.h"

#include "Transform.h"
#include "Collision.h"
#include "Rigidbody.h"

using namespace dx;
using namespace DirectX;

Enemy::Enemy(const XMFLOAT3& position, const XMFLOAT3& size)
{
    mVertices =
    {
        // ¾Õ¸é (Front)
        { XMFLOAT3(-size.x, -size.y, -size.z), XMFLOAT4(1, 0, 0, 1) },
        { XMFLOAT3(-size.x, size.y, -size.z), XMFLOAT4(1, 0, 0, 1) },
        { XMFLOAT3(size.x, size.y, -size.z), XMFLOAT4(1, 0, 0, 1) },

        { XMFLOAT3(-size.x, -size.y, -size.z), XMFLOAT4(1, 0, 0, 1) },
        { XMFLOAT3(size.x, size.y, -size.z), XMFLOAT4(1, 0, 0, 1) },
        { XMFLOAT3(size.x, -size.y, -size.z), XMFLOAT4(1, 0, 0, 1) },

        // µÞ¸é (Back)
        { XMFLOAT3(-size.x, -size.y, size.z), XMFLOAT4(0, 1, 0, 1) },
        { XMFLOAT3(size.x, size.y, size.z), XMFLOAT4(0, 1, 0, 1) },
        { XMFLOAT3(-size.x, size.y, size.z), XMFLOAT4(0, 1, 0, 1) },

        { XMFLOAT3(-size.x, -size.y, size.z), XMFLOAT4(0, 1, 0, 1) },
        { XMFLOAT3(size.x, -size.y, size.z), XMFLOAT4(0, 1, 0, 1) },
        { XMFLOAT3(size.x, size.y, size.z), XMFLOAT4(0, 1, 0, 1) },

        // ¿ÞÂÊ¸é (Left)
        { XMFLOAT3(-size.x, -size.y, size.z), XMFLOAT4(0, 0, 1, 1) },
        { XMFLOAT3(-size.x, size.y, -size.z), XMFLOAT4(0, 0, 1, 1) },
        { XMFLOAT3(-size.x, size.y, size.z), XMFLOAT4(0, 0, 1, 1) },

        { XMFLOAT3(-size.x, -size.y, size.z), XMFLOAT4(0, 0, 1, 1) },
        { XMFLOAT3(-size.x, -size.y, -size.z), XMFLOAT4(0, 0, 1, 1) },
        { XMFLOAT3(-size.x, size.y, -size.z), XMFLOAT4(0, 0, 1, 1) },

        // ¿À¸¥ÂÊ¸é (Right)
        { XMFLOAT3(size.x, -size.y, -size.z), XMFLOAT4(1, 1, 0, 1) },
        { XMFLOAT3(size.x, size.y, size.z), XMFLOAT4(1, 1, 0, 1) },
        { XMFLOAT3(size.x, size.y, -size.z), XMFLOAT4(1, 1, 0, 1) },

        { XMFLOAT3(size.x, -size.y, -size.z), XMFLOAT4(1, 1, 0, 1) },
        { XMFLOAT3(size.x, -size.y, size.z), XMFLOAT4(1, 1, 0, 1) },
        { XMFLOAT3(size.x, size.y, size.z), XMFLOAT4(1, 1, 0, 1) },

        // À­¸é (Top)
        { XMFLOAT3(-size.x, size.y, -size.z), XMFLOAT4(1, 0, 1, 1) },
        { XMFLOAT3(size.x, size.y, size.z), XMFLOAT4(1, 0, 1, 1) },
        { XMFLOAT3(-size.x, size.y, size.z), XMFLOAT4(1, 0, 1, 1) },

        { XMFLOAT3(-size.x, size.y, -size.z), XMFLOAT4(1, 0, 1, 1) },
        { XMFLOAT3(size.x, size.y, -size.z), XMFLOAT4(1, 0, 1, 1) },
        { XMFLOAT3(size.x, size.y, size.z), XMFLOAT4(1, 0, 1, 1) },

        // ¾Æ·§¸é (Bottom)
        { XMFLOAT3(-size.x, -size.y, -size.z), XMFLOAT4(0, 1, 1, 1) },
        { XMFLOAT3(-size.x, -size.y, size.z), XMFLOAT4(0, 1, 1, 1) },
        { XMFLOAT3(size.x, -size.y, size.z), XMFLOAT4(0, 1, 1, 1) },

        { XMFLOAT3(-size.x, -size.y, -size.z), XMFLOAT4(0, 1, 1, 1) },
        { XMFLOAT3(size.x, -size.y, size.z), XMFLOAT4(0, 1, 1, 1) },
        { XMFLOAT3(size.x, -size.y, -size.z), XMFLOAT4(0, 1, 1, 1) },
    };

    IsStatic = true;
    Transform& transform = EmplaceComponent<Transform>();
    transform.Position = position;
    Collision& collision = EmplaceComponent<Collision>(
        Collision::Create<btBoxShape>(btVector3(size.x, size.y, size.z)));
    EmplaceComponent<Rigidbody>(transform, collision, 0.f, 0.3f);
}

void Enemy::Update(const float /*deltaTime*/)
{

}