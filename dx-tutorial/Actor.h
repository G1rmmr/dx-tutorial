#pragma once

#include <vector>

namespace core
{
	class Game;
	class Component;

	class Actor
	{
	public:
		enum class eState
		{
			Active,
			Paused,
			Dead
		};

		Actor(Game* game);
		virtual ~Actor();

		virtual void Update(const float deltaTime) = 0;

		void AddComponent(Component* comp);
		void RemoveComponent(Component* comp);

		inline eState GetState() const
		{
			return mState;
		}

		inline void SetState(const eState state)
		{
			mState = state;
		}

	protected:
		std::vector<Component*> mComponents;
		eState mState;
		Game* mGame;
	};
}