#pragma once
#include <SDL3/SDL.h>
#include <iostream>

struct Rect {
    float x, y, w, h;
};

inline bool pointInRect(float px, float py, const Rect& r) {
    return px >= r.x && px <= r.x + r.w &&
           py >= r.y && py <= r.y + r.h;
}

// Shared SDL lifecycle base for all window classes.
class WindowBase {
protected:
    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;
    int           winW     = 0;
    int           winH     = 0;

    bool initSDL(const char* title, int width, int height, SDL_WindowFlags flags = 0);
    void cleanup();   // Safe to call multiple times.
    void presentFrame(unsigned int delayMs = 16);
};
