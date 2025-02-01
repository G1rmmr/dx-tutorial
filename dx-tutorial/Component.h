#pragma once

namespace core
{
	class Actor;

	class Component
	{
	public:
		Component(Actor* owner, const int updateOrder = 100);
		virtual ~Component();

		virtual void Update(const float deltaTime) = 0;
		
		inline int GetUpdateOrder() const
		{
			return mUpdateOrder;
		}

	protected:
		Actor* mOwner;
		int mUpdateOrder;
	};
}