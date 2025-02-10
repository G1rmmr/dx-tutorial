#include "Player.h"

using namespace core;
using namespace DirectX;

Player::Player()
	: mPos(0.f, 0.f, 0.f)
	, mYaw(0.f)
	, mPitch(0.f)
	, mForward(0.f, 0.f, 1.f)
	, mRight(1.f, 0.f, 0.f)
	, mUp(0.f, 1.f, 0.f)
	, mMovementSpeed(5.f)
	, mMouseSensitivity(0.1f)
{

}

void Player::ProcessMouseMovement(float xOffset, float yOffset)
{
	xOffset *= mMouseSensitivity;
	yOffset *= mMouseSensitivity;

	mYaw -= xOffset;
	mPitch += yOffset;

	if(mPitch > 89.f)
	{
		mPitch = 89.f;
	}

	if(mPitch < -89.f)
	{
		mPitch = -89.f;
	}

	UpdateCameraVectors();
}

void Player::ProcessKeyboard(bool forwardKey, bool backKey, bool leftKey, bool rightKey, float deltaTime)
{
	float vel = mMovementSpeed * deltaTime;

	XMVECTOR pos = XMLoadFloat3(&mPos);
	XMVECTOR forward = XMLoadFloat3(&mForward);
	XMVECTOR right = XMLoadFloat3(&mRight);

	if(forwardKey)
	{
		pos = XMVectorAdd(pos, XMVectorScale(forward, vel));
	}
		
	if(backKey)
	{
		pos = XMVectorSubtract(pos, XMVectorScale(forward, vel));
	}
		
	if(leftKey)
	{
		pos = XMVectorSubtract(pos, XMVectorScale(right, vel));
	}

	if(rightKey)
	{
		pos = XMVectorAdd(pos, XMVectorScale(right, vel));
	}

	DirectX::XMStoreFloat3(&mPos, pos);
}

XMMATRIX Player::GetViewMatrix() const
{
	XMVECTOR pos = XMLoadFloat3(&mPos);
	XMVECTOR forward = XMLoadFloat3(&mForward);
	XMVECTOR up = XMLoadFloat3(&mUp);

	return XMMatrixLookAtLH(pos, XMVectorAdd(pos, forward), up);
}

void Player::UpdateCameraVectors()
{
	float radPitch = XMConvertToRadians(mPitch);
	float radYaw = XMConvertToRadians(mYaw);

	XMVECTOR forward;
	forward = XMVectorSet(
		cosf(radYaw) * cosf(radPitch),
		sinf(radPitch),
		sinf(radYaw) * cosf(radPitch),
		0.f);

	forward = XMVector3Normalize(forward);

	XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
	XMVECTOR right = XMVector3Normalize(XMVector3Cross(up, forward));

	up = XMVector3Cross(forward, right);

	XMStoreFloat3(&mForward, forward);
	XMStoreFloat3(&mRight, right);
	XMStoreFloat3(&mUp, up);
}