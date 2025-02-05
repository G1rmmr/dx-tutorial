#pragma once

#include <windows.h>

#include <vector>

namespace core
{
	class Actor;
	class Renderer;

	class Game
	{
	public:
		Game();
		~Game();

		bool Initialize(HINSTANCE hInstance, int width, int height, int nCmdShow);
		void Run();
		void Cleanup();

		void AddActor(Actor* actor);
		void RemoveActor(Actor* actor);

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
		
		void loadData();
		void unloadData();

		std::vector<Actor*> mActors;
		std::vector<Actor*> mPendingActors;

		Renderer* mRenderer;

		HWND mHwnd;
		HINSTANCE mHInstance;

		LONG mTicksCount;

		int mScreenWidth;
		int mScreenHeight;

		bool mIsRunning;
		bool mUpdatingActors;
	};
}