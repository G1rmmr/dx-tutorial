#pragma once
#include "Component.h"
#include <bullet/btBulletCollisionCommon.h>
#include <concepts>
#include <memory>
#include <utility>
namespace dx
{
    class Rigidbody;
    class Collision : public Component
    {
    public:
        Collision(const Collision&) = delete;
        Collision& operator=(const Collision&) = delete;
        Collision(Collision&&) noexcept = default;
        Collision& operator=(Collision&&) noexcept = default;
        template<std::derived_from<btCollisionShape> Shape, typename... Args>
        static Collision Create(Args&&... args)
        {
            return Collision(std::make_shared<Shape>(std::forward<Args>(args)...));
        }
        btCollisionShape* GetShape() noexcept { return mShape.get(); }
        const btCollisionShape* GetShape() const noexcept { return mShape.get(); }
    private:
        friend class Rigidbody;
        // The body retains the shape regardless of component destruction order.
        std::shared_ptr<btCollisionShape> mShape;
        explicit Collision(std::shared_ptr<btCollisionShape> shape) : mShape(std::move(shape)) {}
    };
}
