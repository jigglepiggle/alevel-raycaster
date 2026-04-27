#include "BaseWindow.h"

bool WindowBase::initSDL(const char* title, int width, int height, SDL_WindowFlags flags) {
    winW = width;
    winH = height;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return false;
    }
    window = SDL_CreateWindow(title, width, height, flags);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }
    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        window = nullptr;
        SDL_Quit();
        return false;
    }
    return true;
}

void WindowBase::cleanup() {
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window)   { SDL_DestroyWindow(window);     window   = nullptr; }
    SDL_Quit();
}

void WindowBase::presentFrame(unsigned int delayMs) {
    SDL_RenderPresent(renderer);
    SDL_Delay(delayMs);
}
