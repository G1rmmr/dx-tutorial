#pragma once

#include <d2d1_1.h>
#include <dwrite.h>
#include <string>
#include <wrl/client.h>

namespace dx
{
    class Renderer;

    class UI
    {
    public:
        UI();
        ~UI();

        bool Initialize(Renderer* renderer, int width, int height);
        void Cleanup();
        void ReleaseTarget();
        bool Resize(int width, int height);

        void Begin();

        void Draw(const std::wstring& text, float x, float y, float size);

        void End();

    private:
        bool createRenderTarget();

    private:
        Microsoft::WRL::ComPtr<ID2D1Factory1> mD2DFactory;
        Microsoft::WRL::ComPtr<IDWriteFactory> mDWriteFactory;
        Microsoft::WRL::ComPtr<ID2D1RenderTarget> mRenderTarget;
        Microsoft::WRL::ComPtr<IDWriteTextFormat> mTextFormat;
        Microsoft::WRL::ComPtr<ID2D1SolidColorBrush> mColorBrush;

        Renderer* mRenderer = nullptr;

        int mWidth = 0;
        int mHeight = 0;
    };
}
