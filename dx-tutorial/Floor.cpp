#include "Floor.h"

#include "Transform.h"
#include "Collision.h"
#include "Rigidbody.h"

using namespace DirectX;
using namespace dx;

Floor::Floor(
    const XMFLOAT3& position,
    const XMFLOAT3& size,
    const XMFLOAT4& color)
{
    mVertices =
    {
        { XMFLOAT3(size.x, size.y, size.z), color},
        { XMFLOAT3(size.x, size.y, -size.z), color},
        { XMFLOAT3(-size.x, size.y, -size.z), color},
        { XMFLOAT3(-size.x, size.y, size.z), color },
        { XMFLOAT3(size.x, size.y, size.z), color },
        { XMFLOAT3(-size.x, size.y, -size.z), color }
    };

    IsStatic = true;
    Transform& transform = EmplaceComponent<Transform>();
    transform.Position = position;
    Collision& collision = EmplaceComponent<Collision>(
        Collision::Create<btBoxShape>(btVector3(size.x, size.y, size.z)));
    EmplaceComponent<Rigidbody>(transform, collision, 0.f, 0.3f);
}