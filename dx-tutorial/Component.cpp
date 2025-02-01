#include "Component.h"
#include "Actor.h"

using namespace core;

Component::Component(Actor* owner, const int updateOrder)
	: mOwner(owner)
	, mUpdateOrder(updateOrder)
{
	mOwner->AddComponent(this);
}

Component::~Component()
{
	mOwner->RemoveComponent(this);
}