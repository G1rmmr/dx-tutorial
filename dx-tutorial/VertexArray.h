#pragma once

#include <vector>
#include <string>
#include <d3d11.h>

namespace core
{
    class VertexArray;
    class Texture;

    class Mesh
    {
    public:
        Mesh();
        ~Mesh();

        bool Load(const std::wstring& fileName, ID3D11Device* device, ID3D11DeviceContext* context);
        void Unload();

        Texture* GetTexture(size_t index) const;

        inline VertexArray* GetVertexArray() const
        {
            return mVtxArr;
        }

        inline const std::string& GetShaderName() const
        {
            return mShaderPath;
        }

        inline float GetRadius() const
        {
            return mRadius;
        }

        inline float GetSpecPower() const
        {
            return mSpecPower;
        }

    private:
        std::vector<Texture*> mTextures;
        
        VertexArray* mVtxArr;
        
        std::string mShaderPath;
        
        float mRadius;
        float mSpecPower;
    };
}