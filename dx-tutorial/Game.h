#pragma once

#include <windows.h>

#include <vector>

#include "Renderer.h"
#include "Player.h"

namespace core
{
	class Game
	{
	public:
		Game();
		~Game();

		bool Initialize(HINSTANCE hInstance, int width, int height, int nCmdShow);
		void Run();
		void Cleanup();

		inline Renderer* GetRenderer() const
		{
			return mRenderer;
		}

		inline bool IsRunning() const
		{
			return mIsRunning;
		}

	private:
		static LRESULT CALLBACK sWindowProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool createWindow(HINSTANCE hInstance, int width, int height, int nCmdShow);
		
		void processInput();
		void update();
		void render();

		Player mPlayer;
		HWND mHwnd;
		
		HINSTANCE mHInstance;

		Renderer* mRenderer;

		DWORD mTicksCount;

		int mScreenWidth;
		int mScreenHeight;

		bool mIsRunning;
	};
}