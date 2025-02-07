#pragma once

#include <DirectXMath.h>

namespace core
{
	class Player
	{
	public:
        Player();

        void ProcessMouseMovement(float xOffset, float yOffset);
        void ProcessKeyboard(bool forwardKey, bool backKey, bool leftKey, bool rightKey, float deltaTime);

        DirectX::XMMATRIX GetViewMatrix() const;
		void UpdateCameraVectors();

	private:
		DirectX::XMFLOAT3 mPos;

		DirectX::XMFLOAT3 mForward;
		DirectX::XMFLOAT3 mRight;
		DirectX::XMFLOAT3 mUp;

		float mYaw;
		float mPitch;

		float mMovementSpeed;
		float mMouseSensitivity;
	};
}