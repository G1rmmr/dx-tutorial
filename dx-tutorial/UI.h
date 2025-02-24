#pragma once

#include <d2d1_1.h>
#include <dwrite.h>
#include <string>

namespace core
{
    class Renderer;

    class UI
    {
    public:
        UI();
        ~UI();

        bool Initialize(Renderer* renderer, int width, int height);
        void Cleanup();

        void Begin();

        void Draw(const std::wstring& text, float x, float y, float size);

        void End();

    private:
        bool createRenderTarget();

    private:
        ID2D1Factory1* mD2DFactory;
        IDWriteFactory* mDWriteFactory;
        ID2D1RenderTarget* mRenderTarget;
        IDWriteTextFormat* mTextFormat;
        ID2D1SolidColorBrush* mColorBrush;

        class Renderer* mRenderer; 
        
        int mWidth;
        int mHeight;
    };
}