#pragma once

#include <windows.h>

static const int WIN_WIDTH = 1024;
static const int WIN_HEIGHT = 768;
static const int THICKNESS = 15;
static const float PADDLE_HEIGHT = 100.f;

struct Vec2
{
	float X;
	float Y;
};

class Game
{
public:
	Game();
	~Game() = default;

	bool Initialize(HINSTANCE hInstance);
	void Cleanup();

	static LRESULT CALLBACK sWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

private:
	void onCreate(HWND hwnd, CREATESTRUCT* create);
	void onDestroy();
	void onPaint(HDC hdc, PAINTSTRUCT& ps);
	void onTimer(UINT_PTR timerId);
	void onKeyDown(WPARAM wParam);
	void onKeyUp(WPARAM wParam);

	void updateGame(float deltaTime);
	void render(HDC hdc);

	LARGE_INTEGER mCount;
	LARGE_INTEGER mFreq;

	HWND mHwnd;

	Vec2 mPadPos;
	Vec2 mBallPos;
	Vec2 mBallVel;
	
	int mPadDir;
	bool bIsRunning;

	bool mKeyW;
	bool mKeyS;
};