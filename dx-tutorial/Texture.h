#pragma once

#include <d3d11.h>
#include <string>

namespace core
{
	class Texture
	{
	public:
		Texture();
		~Texture();

		bool Load(ID3D11DeviceContext* context, const std::wstring& path);
		void Unload();

		void SetActive(ID3D11DeviceContext* context, int slot = 0);

		inline int GetWidth() const
		{
			return mWidth;
		}

		inline int GetHeight() const
		{
			return mHeight;
		}

	private:
		ID3D11Texture2D* mTex;
		ID3D11ShaderResourceView* mView;

		int mWidth;
		int mHeight;
	};
}