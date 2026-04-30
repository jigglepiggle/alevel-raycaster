#include "BaseWindow.h"

bool BaseWindow::initSDL(const char* title, int width, int height, SDL_WindowFlags flags) {
    winW = width;
    winH = height;

    window = SDL_CreateWindow(title, width, height, flags);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }

    renderer = SDL_CreateRenderer(window, nullptr);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        window = nullptr;
        return false;
    }

    return true;
}

void BaseWindow::cleanup() {
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window)   { SDL_DestroyWindow(window);     window   = nullptr; }
}

void BaseWindow::presentFrame(unsigned int delayMs) {
    SDL_RenderPresent(renderer);
    SDL_Delay(delayMs);
}
