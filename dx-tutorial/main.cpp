#include "Game.h"
#include "Window.h"

#include <cstdlib>
#include <exception>
#include <filesystem>
#include <stdexcept>
#include <vector>

namespace
{
    void useExecutableDirectory()
    {
        std::vector<wchar_t> filename(32768);
        const DWORD length = GetModuleFileNameW(nullptr, filename.data(),
            static_cast<DWORD>(filename.size()));
        if (length == 0 || length >= filename.size())
        {
            throw std::runtime_error("Cannot locate the executable directory");
        }
        std::filesystem::current_path(std::filesystem::path(filename.data()).parent_path());
    }
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE, _In_ LPSTR, _In_ int nCmdShow)
{
    try
    {
        useExecutableDirectory();
        dx::Window window(L"Planned Defence", hInstance, 1280, 720, nCmdShow);
        dx::Game game(window);
        game.Run();
        return EXIT_SUCCESS;
    }
    catch (const std::exception& error)
    {
        MessageBoxA(nullptr, error.what(), "Planned Defence", MB_OK | MB_ICONERROR);
        return EXIT_FAILURE;
    }
}
