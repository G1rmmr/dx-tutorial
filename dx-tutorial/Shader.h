#pragma once

#include <d3d11.h>
#include <d3dcompiler.h>
#include <DirectXMath.h>

#include <string>

namespace core
{
	struct CBData
	{
		DirectX::XMFLOAT4X4 Mat;
		DirectX::XMFLOAT4 Param;
	};

	class Shader
	{
	public:
		Shader();
		~Shader();

		bool Load(ID3D11Device* device,
			const std::wstring& vertex_path,
			const std::wstring& pixel_path);

		void Unload();

		void SetActive(ID3D11DeviceContext* context);

		void SetMat(ID3D11DeviceContext* context,
			const char* name, const DirectX::XMMATRIX& matrix);

		void SetVec(ID3D11DeviceContext* context,
			const char* name, const DirectX::XMFLOAT3& vector);

		void SetFloat(ID3D11DeviceContext* context,
			const char* name, const float val);

	private:
		ID3DBlob* compile(
			const std::wstring& fileName,
			const std::string& entryPoint,
			const std::string& shaderModel);

		ID3D11VertexShader* mVtxShader;
		ID3D11PixelShader* mPixShader;

		ID3D11InputLayout* mInputLayout;
		ID3D11Buffer* mConstBuff;

		CBData*  mCBData;

		bool mIsLoaded;
	};
}