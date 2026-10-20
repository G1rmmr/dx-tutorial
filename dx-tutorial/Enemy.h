#pragma once

#include "Object.h"
#include "Vertex.h"

#include <DirectXMath.h>
#include <vector>

namespace dx
{
	class Enemy : public Object
	{
	public:
		Enemy(const DirectX::XMFLOAT3& position, const DirectX::XMFLOAT3& size);

		void Update(const float deltaTime) override;

		inline const std::vector<Vertex>& GetVertices() const
		{
			return mVertices;
		}

	private:
		std::vector<Vertex> mVertices;
	};
}