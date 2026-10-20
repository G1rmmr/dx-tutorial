#pragma once
#include <concepts>
#include <memory>
#include <stdexcept>
#include <typeindex>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include "Component.h"
namespace dx
{
    class Object
    {
    public:
        Object() = default;
        virtual ~Object() = default;
        bool IsStatic = false;
        virtual void Update(float deltaTime) = 0;

        template<std::derived_from<Component> T>
        T* GetComponent() noexcept
        {
            const auto iterator = mComponents.find(std::type_index(typeid(T)));
            return iterator == mComponents.end() ? nullptr : static_cast<T*>(iterator->second.get());
        }

        template<std::derived_from<Component> T>
        const T* GetComponent() const noexcept
        {
            const auto iterator = mComponents.find(std::type_index(typeid(T)));
            return iterator == mComponents.end() ? nullptr : static_cast<const T*>(iterator->second.get());
        }

        template<std::derived_from<Component> T, typename... Args>
        T& EmplaceComponent(Args&&... args)
        {
            const std::type_index type(typeid(T));
            if (mComponents.contains(type))
            {
                throw std::logic_error("Component type already exists");
            }
            auto component = std::make_unique<T>(std::forward<Args>(args)...);
            T& result = *component;
            mComponents.emplace(type, std::move(component));
            return result;
        }

        template<std::derived_from<Component> T>
            requires std::copy_constructible<T>
        T& AddComponent(const T& component)
        {
            return EmplaceComponent<T>(component);
        }

        template<typename T>
            requires std::derived_from<std::remove_cvref_t<T>, Component>
        std::remove_cvref_t<T>& AddComponent(T&& component)
        {
            return EmplaceComponent<std::remove_cvref_t<T>>(std::forward<T>(component));
        }
    protected:
        std::unordered_map<std::type_index, std::unique_ptr<Component>> mComponents;
    };
}
