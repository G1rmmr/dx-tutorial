#include "Game.h"

#include <cmath>

Game::Game()
	: mHwnd(nullptr)
	, bIsRunning(true)
	, mKeyW(false)
	, mKeyS(false)
{

}

bool Game::Initialize(HINSTANCE hInstance)
{
	const wchar_t* CLASS_NAME = L"dx-tutorial";

	WNDCLASS wc = {};
	wc.lpfnWndProc = Game::sWindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	
	if(!RegisterClass(&wc))
	{
		MessageBox(nullptr, L"RegisterClass failed!", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	mHwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		CLASS_NAME,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		WIN_WIDTH,
		WIN_HEIGHT,
		nullptr,
		nullptr,
		hInstance,
		this
	);
	
	if(!mHwnd)
	{
		MessageBox(nullptr, L"CreateWindowEx failed!", L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	ShowWindow(mHwnd, SW_SHOW);

	QueryPerformanceFrequency(&mFreq);
	QueryPerformanceCounter(&mCount);

	mPadPos = {10.f, WIN_HEIGHT * 0.5f};
	mBallPos = {WIN_WIDTH * 0.5f,  WIN_HEIGHT * 0.5f};
	mBallVel = {-200.f, 235.f};

	SetTimer(mHwnd, 1, 16, nullptr);
	return true;
}

LRESULT CALLBACK Game::sWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	Game* game = reinterpret_cast<Game*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

	switch(uMsg)
	{
	case WM_CREATE:
	{
		CREATESTRUCT* cs = reinterpret_cast<CREATESTRUCT*>(lParam);
		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(cs->lpCreateParams));
		return 0;
	}
	case WM_DESTROY:
		if(game)
		{
			game->onDestroy();
		}
		return 0;

	case WM_PAINT:
	{
		if(game && game->bIsRunning)
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			game->onPaint(hdc, ps);
			EndPaint(hwnd, &ps);
		}
		else
		{
			PAINTSTRUCT ps;
			BeginPaint(hwnd, &ps);
			EndPaint(hwnd, &ps);
		}
		return 0;
	}
	case WM_TIMER:
	{
		if(game && game->bIsRunning)
		{
			game->onTimer(wParam);
		}
		return 0;
	}
	case WM_KEYDOWN:
		if(game && game->bIsRunning)
		{
			game->onKeyDown(wParam);
		}
		return 0;
	case WM_KEYUP:
		if(game && game->bIsRunning)
		{
			game->onKeyUp(wParam);
		}
		return 0;

	default:
		break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

void Game::onCreate(HWND, CREATESTRUCT*)
{

}

void Game::onDestroy()
{
	PostQuitMessage(0);
	bIsRunning = false;
}

void Game::onPaint(HDC hdc, PAINTSTRUCT&)
{
	render(hdc);
}

void Game::onTimer(UINT_PTR timerId)
{
	if(timerId != 1)
	{
		return;
	}
	
	LARGE_INTEGER currCount;
	QueryPerformanceCounter(&currCount);

	double deltaTime = double(currCount.QuadPart - mCount.QuadPart)
		/ double(mFreq.QuadPart);

	if(deltaTime > 0.05f)
	{
		deltaTime = 0.05f;
	}
	mCount = currCount;

	updateGame(static_cast<float>deltaTime);

	InvalidateRect(mHwnd, nullptr, FALSE);
}

void Game::onKeyDown(WPARAM wParam)
{
	switch(wParam)
	{
	case VK_ESCAPE:
		PostQuitMessage(0);
		bIsRunning = false;
		break;

	case 'W':
		mKeyW = true;
		break;

	case 'S':
		mKeyS = true;
		break;

	default:
		break;
	}
}

void Game::onKeyUp(WPARAM wParam)
{
	switch(wParam)
	{
	case 'W':
		mKeyW = false;
		break;

	case 'S':
		mKeyS = false;
		break;

	default:
		break;
	}
}

void Game::updateGame(float deltaTime)
{
	float padDir = 0.f;
	
	if(mKeyW)
	{
		padDir -= 1.f;
	}

	if(mKeyS)
	{
		padDir += 1.f;
	}

	if(padDir != 0.f)
	{
		mPadPos.Y += padDir * 300.f * deltaTime;

		float halfPad = PADDLE_HEIGHT * 0.5f;
		if(mPadPos.Y < halfPad + THICKNESS)
		{
			mPadPos.Y = halfPad + THICKNESS;
		}
		else if(mPadPos.Y > (WIN_HEIGHT - halfPad - THICKNESS))
		{
			mPadPos.Y = WIN_HEIGHT - halfPad - THICKNESS;
		}
	}

	mBallPos.X += mBallVel.X * deltaTime;
	mBallPos.Y += mBallVel.Y * deltaTime;

	float diff = std::fabs(mPadPos.Y - mBallPos.Y);

	if(diff <= PADDLE_HEIGHT * 0.5f &&
		mBallPos.X <= 25.f && mBallPos.X >= 20.f &&
		mBallVel.X < 0.f)
	{
		mBallVel.X *= -1.f;
	}
	else if(mBallPos.X <= 0.f)
	{
		bIsRunning = false;
		PostQuitMessage(0);
	}
	else if(mBallPos.X >= (WIN_WIDTH - THICKNESS) && mBallVel.X > 0.f)
	{
		mBallVel.X *= -1.f;
	}

	if(mBallPos.Y <= THICKNESS && mBallVel.Y < 0.f)
	{
		mBallVel.Y *= -1.f;
	}
	else if(mBallPos.Y >= (WIN_HEIGHT - THICKNESS) && mBallVel.Y > 0.f)
	{
		mBallVel.Y *= -1.f;
	}
}

void Game::render(HDC hdc)
{
	HBRUSH blueBrush = CreateSolidBrush(RGB(0x00, 0x00, 0xFF));
	RECT bg = {0, 0, WIN_WIDTH, WIN_HEIGHT};
	FillRect(hdc, &bg, blueBrush);
	DeleteObject(blueBrush);

	HBRUSH hBrush = CreateSolidBrush(RGB(0xFF, 0xFF, 0xFF));
	
	RECT topWall{0, 0, WIN_WIDTH, THICKNESS};
	FillRect(hdc, &topWall, hBrush);

	RECT bottomWall{0, WIN_HEIGHT - THICKNESS, WIN_WIDTH, WIN_HEIGHT};
	FillRect(hdc, &bottomWall, hBrush);

	RECT rightWall{WIN_WIDTH - THICKNESS, 0, WIN_WIDTH, WIN_HEIGHT};
	FillRect(hdc, &rightWall, hBrush);

	RECT paddle{
		static_cast<LONG>(mPadPos.X),
		static_cast<LONG>(mPadPos.Y - PADDLE_HEIGHT * 0.5f),
		static_cast<LONG>(mPadPos.X + THICKNESS),
		static_cast<LONG>(mPadPos.Y + PADDLE_HEIGHT * 0.5f)
	};
	FillRect(hdc, &paddle, hBrush);

	RECT ball{
		static_cast<LONG>(mBallPos.X - THICKNESS * 0.5f),
		static_cast<LONG>(mBallPos.Y - THICKNESS * 0.5f),
		static_cast<LONG>(mBallPos.X + THICKNESS * 0.5f),
		static_cast<LONG>(mBallPos.Y + THICKNESS * 0.5f)
	};
	FillRect(hdc, &ball, hBrush);

	DeleteObject(hBrush);
}

void Game::Cleanup()
{
	if(mHwnd)
	{
		KillTimer(mHwnd, 1);
		DestroyWindow(mHwnd);
		mHwnd = nullptr;
	}
}