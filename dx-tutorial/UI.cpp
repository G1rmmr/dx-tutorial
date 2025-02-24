#include "UI.h"
#include "Renderer.h"
#include "Pipeline.h"

#include <dxgi.h>

#include <d2d1.h>
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "dwrite.lib")

using namespace core;

UI::UI()
    : mD2DFactory(nullptr)
    , mDWriteFactory(nullptr)
    , mRenderTarget(nullptr)
    , mTextFormat(nullptr)
    , mColorBrush(nullptr)
    , mRenderer(nullptr)
    , mWidth(0)
    , mHeight(0)
{

}

UI::~UI()
{
    Cleanup();
}

bool UI::Initialize(Renderer* renderer, int width, int height)
{
    mRenderer = renderer;
    mWidth = width;
    mHeight = height;

    HRESULT hr = D2D1CreateFactory(
        D2D1_FACTORY_TYPE_SINGLE_THREADED,
        IID_PPV_ARGS(&mD2DFactory));

    if(FAILED(hr))
    {
        MessageBox(nullptr, TEXT("D2D1factory not initialized!"), TEXT("LOG"), MB_OK);
        return false;
    }
    
    hr = DWriteCreateFactory(
        DWRITE_FACTORY_TYPE_SHARED,
        __uuidof(IDWriteFactory),
        reinterpret_cast<IUnknown**>(&mDWriteFactory));

    if(FAILED(hr))
    {
        MessageBox(nullptr, TEXT("DWritefactory not initialized!"), TEXT("LOG"), MB_OK);
        return false;
    }

    hr = mDWriteFactory->CreateTextFormat(
        L"Malgun Gothic",
        nullptr,
        DWRITE_FONT_WEIGHT_NORMAL,
        DWRITE_FONT_STYLE_NORMAL,
        DWRITE_FONT_STRETCH_NORMAL,
        32.0f,
        L"ko-KR",
        &mTextFormat);

    if(FAILED(hr))
    {
        MessageBox(nullptr, TEXT("Format not initialized!"), TEXT("LOG"), MB_OK);
        return false;
    }

    mTextFormat->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_LEADING);
    mTextFormat->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_NEAR);

    if(!createRenderTarget())
    {
        MessageBox(nullptr, TEXT("Render target not initialized!"), TEXT("LOG"), MB_OK);
        return false;
    }

    hr = mRenderTarget->CreateSolidColorBrush(
        D2D1::ColorF(D2D1::ColorF::White),
        &mColorBrush);

    if(FAILED(hr))
    {
        MessageBox(nullptr, TEXT("brush not initialized!"), TEXT("LOG"), MB_OK);
        return false;
    }
    return true;
}

bool UI::createRenderTarget()
{
    if(!mRenderer || !mRenderer->GetPipeline())
    {
        MessageBox(nullptr, TEXT("Renderer not initialized!"), TEXT("LOG"), MB_OK);
        return false;
    }

    ID3D11Texture2D* backBuffer = nullptr;
    HRESULT hr = mRenderer->GetPipeline()->GetSwapChain()->GetBuffer(
        0,
        IID_PPV_ARGS(&backBuffer));

    if(FAILED(hr))
    {
        MessageBox(nullptr, TEXT("buffer not initialized!"), TEXT("LOG"), MB_OK);
        return false;
    }
        
    IDXGISurface* dxgiSurface = nullptr;
    hr = backBuffer->QueryInterface(IID_PPV_ARGS(&dxgiSurface));
    
    backBuffer->Release();
    backBuffer = nullptr;

    if(FAILED(hr))
    {
        MessageBox(nullptr, TEXT("surface not initialized!"), TEXT("LOG"), MB_OK);

        if(dxgiSurface)
        {
            dxgiSurface->Release();
        }
        return false;
    }

    D2D1_RENDER_TARGET_PROPERTIES rtProps =
        D2D1::RenderTargetProperties(
            D2D1_RENDER_TARGET_TYPE_DEFAULT,
            D2D1::PixelFormat(DXGI_FORMAT_UNKNOWN, D2D1_ALPHA_MODE_PREMULTIPLIED),
            0.0f,
            0.0f);

    hr = mD2DFactory->CreateDxgiSurfaceRenderTarget(
        dxgiSurface,
        &rtProps,
        &mRenderTarget);

    dxgiSurface->Release();
    dxgiSurface = nullptr;

    if(FAILED(hr))
    {
        MessageBox(nullptr, TEXT("Render target not initialized!"), TEXT("LOG"), MB_OK);
        return false;
    }
    return true;
}

void UI::Cleanup()
{
    if(mColorBrush)
    {
        mColorBrush->Release();
        mColorBrush = nullptr;
    }

    if(mTextFormat)
    {
        mTextFormat->Release();
        mTextFormat = nullptr;
    }

    if(mRenderTarget)
    {
        mRenderTarget->Release();
        mRenderTarget = nullptr;
    }

    if(mDWriteFactory)
    {
        mDWriteFactory->Release();
        mDWriteFactory = nullptr;
    }

    if(mD2DFactory)
    {
        mD2DFactory->Release();
        mD2DFactory = nullptr;
    }

    mRenderer = nullptr;
}

void UI::Begin()
{
    if(mRenderTarget)
    {
        mRenderTarget->BeginDraw();
    }
}

void UI::Draw(const std::wstring& text, float x, float y, float /*size*/)
{
    if(!mRenderTarget || !mTextFormat || !mColorBrush)
    {
        return;
    }

    D2D1_RECT_F layoutRect = D2D1::RectF(x, y, x + 500.0f, y + 100.0f);

    mRenderTarget->DrawTextW(
        text.c_str(),
        static_cast<UINT32>(text.size()),
        mTextFormat,
        layoutRect,
        mColorBrush);
}

void UI::End()
{
    if(mRenderTarget)
    {
        HRESULT hr = mRenderTarget->EndDraw();
        if(hr == D2DERR_RECREATE_TARGET)
        {
            mRenderTarget->Release();
            mRenderTarget = nullptr;
            createRenderTarget();
        }
    }
}