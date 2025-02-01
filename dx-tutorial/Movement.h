#pragma once

#include "Component.h"

class Movement final : public core::Component
{
public:
	void Update(const float deltaTime) override final;

	inline float GetAngSpeed() const
	{
		return mAngSpeed;
	}

	inline float GetForwSpeed() const
	{
		return mForwSpeed;
	}
	
	inline void SetAngSpeed(const float angSpeed)
	{
		mAngSpeed = angSpeed;
	}
	
	inline void SetForwSpeed(const float forwSpeed)
	{
		mForwSpeed = forwSpeed;
	}

private:
	float mAngSpeed;
	float mForwSpeed;
};