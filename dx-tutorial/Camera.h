#pragma once

#include "Actor.h"

class Camera final : public core::Actor
{
public:
	void Update(const float deltaTime) override final;
};