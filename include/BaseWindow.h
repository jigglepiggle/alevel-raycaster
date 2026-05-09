#pragma once
#include <SDL3/SDL.h>
#include <iostream>

// Simple axis-aligned rectangle used for button hit-testing and drawing.
struct Rect {
    float x, y, w, h;
};

// Returns true if point (px, py) lies inside the given rectangle.
inline bool pointInRect(float px, float py, const Rect& r) {
    return px >= r.x && px <= r.x + r.w &&
           py >= r.y && py <= r.y + r.h;
}

// Base class that all window types inherit from.
// Holds the SDL window and renderer pointers and provides shared lifecycle helpers.    // Skills Audit Group A: Complex user-defined use of OOP model (inheritance)
class BaseWindow {
protected:
    // SDL handles — initialised to null and set by initSDL.
    SDL_Window*   window   = nullptr;
    SDL_Renderer* renderer = nullptr;
    // Window dimensions in pixels.
    int           winW     = 0;
    int           winH     = 0;

    // Creates the SDL window and renderer; returns false on failure.
    bool initSDL(const char* title, int width, int height, SDL_WindowFlags flags = 0);
    // Destroys the renderer and window; safe to call more than once.
    void cleanup();
    // Presents the rendered frame and optionally delays to cap frame rate.
    void presentFrame(unsigned int delayMs = 16);
};
