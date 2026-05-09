#include "BaseWindow.h"

bool BaseWindow::initSDL(const char* title, int width, int height, SDL_WindowFlags flags) {
    // Store dimensions so subclasses can reference them without needing SDL calls.
    winW = width;    // Skills Audit Group B: Records (struct/class member variables storing window state)
    winH = height;

    // Create the OS window; print the SDL error and return false on failure.
    window = SDL_CreateWindow(title, width, height, flags);
    if (!window) {
        std::cerr << "SDL_CreateWindow failed: " << SDL_GetError() << std::endl;
        return false;
    }

    // Create a hardware-accelerated renderer attached to the window.
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
    // Destroy renderer and window in the correct order to avoid dangling dependencies.
    if (renderer) { SDL_DestroyRenderer(renderer); renderer = nullptr; }
    if (window)   { SDL_DestroyWindow(window);     window   = nullptr; }
}

void BaseWindow::presentFrame(unsigned int delayMs) {
    // Flip the back buffer to the screen, then sleep to cap the frame rate.
    SDL_RenderPresent(renderer);
    SDL_Delay(delayMs);    // Skills Audit Group B: Simple scientific/mathematical model (fixed frame-rate timing)
}
