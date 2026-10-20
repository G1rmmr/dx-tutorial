#include "UI.h"
#include "Renderer.h"

#include <d2d1.h>
#include <dxgi.h>
#include <algorithm>

#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using namespace dx;
using Microsoft::WRL::ComPtr;

UI::UI() = default;
UI::~UI() = default;

bool UI::Initialize(Renderer* renderer, int width, int height)
{
    Cleanup();
    if (!renderer || width <= 0 || height <= 0)
    {
        return false;
    }
    mRenderer = renderer;
    mWidth = width;
    mHeight = height;
    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory1), nullptr,
        reinterpret_cast<void**>(mD2DFactory.GetAddressOf()));
    if (FAILED(hr))
    {
        Cleanup();
        return false;
    }
    hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(mDWriteFactory.GetAddressOf()));
    if (FAILED(hr))
    {
        Cleanup();
        return false;
    }
    hr = mDWriteFactory->CreateTextFormat(L"Malgun Gothic", nullptr,
        DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL, 32.0f, L"ko-KR", mTextFormat.GetAddressOf());
    if (FAILED(hr))
    {
        Cleanup();
        return false;
    }
    mTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    mTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);
    if (!createRenderTarget())
    {
        Cleanup();
        return false;
    }
    return true;
}

bool UI::createRenderTarget()
{
    ReleaseTarget();
    if (!mRenderer || !mD2DFactory || mWidth <= 0 || mHeight <= 0)
    {
        return false;
    }
    ComPtr<IDXGISurface> surface;
    HRESULT hr = mRenderer->GetSwapChain()->GetBuffer(0, IID_PPV_ARGS(surface.GetAddressOf()));
    if (FAILED(hr))
    {
        return false;
    }
    const auto properties = D2D1::RenderTargetProperties(
        D2D1_RENDER_TARGET_TYPE_DEFAULT,
        D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_IGNORE));
    hr = mD2DFactory->CreateDxgiSurfaceRenderTarget(
        surface.Get(), &properties, mRenderTarget.GetAddressOf());
    if (FAILED(hr))
    {
        return false;
    }
    hr = mRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White), mColorBrush.GetAddressOf());
    if (FAILED(hr))
    {
        ReleaseTarget();
        return false;
    }
    return true;
}

void UI::ReleaseTarget()
{
    mColorBrush.Reset();
    mRenderTarget.Reset();
}

bool UI::Resize(int width, int height)
{
    mWidth = width;
    mHeight = height;
    return createRenderTarget();
}

void UI::Cleanup()
{
    ReleaseTarget();
    mTextFormat.Reset();
    mDWriteFactory.Reset();
    mD2DFactory.Reset();
    mRenderer = nullptr;
    mWidth = 0;
    mHeight = 0;
}

void UI::Begin()
{
    if (!mRenderTarget && !createRenderTarget())
    {
        return;
    }
    mRenderer->GetContext()->OMSetRenderTargets(0, nullptr, nullptr);
    mRenderer->GetContext()->Flush();
    mRenderTarget->BeginDraw();
}

void UI::Draw(const std::wstring& text, float x, float y, float size)
{
    if (!mRenderTarget || !mTextFormat || !mColorBrush)
    {
        return;
    }
    ComPtr<IDWriteTextLayout> layout;
    const HRESULT hr = mDWriteFactory->CreateTextLayout(text.c_str(),
        static_cast<UINT32>(text.size()), mTextFormat.Get(),
        (std::max)(1.0f, static_cast<float>(mWidth) - x),
        (std::max)(1.0f, static_cast<float>(mHeight) - y), layout.GetAddressOf());
    if (FAILED(hr))
    {
        return;
    }
    if (size > 0)
    {
        layout->SetFontSize(size, DWRITE_TEXT_RANGE{0, static_cast<UINT32>(text.size())});
    }
    mRenderTarget->DrawTextLayout(D2D1::Point2F(x, y), layout.Get(), mColorBrush.Get());
}

void UI::End()
{
    if (!mRenderTarget)
    {
        return;
    }
    if (mRenderTarget->EndDraw() == D2DERR_RECREATE_TARGET)
    {
        // Recreate both resources in the new render target's resource domain.
        ReleaseTarget();
    }
}
