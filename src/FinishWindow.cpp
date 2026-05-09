#include "FinishWindow.h"
#include "UIHelpers.h"
#include <iomanip>
#include <sstream>

// Store the elapsed time and set the fixed window size.
FinishWindow::FinishWindow(double elapsed) : elapsedSeconds(elapsed) {    // Skills Audit Group B: Records (struct/class storing completion state)
    winW = 600;
    winH = 420;
}

bool FinishWindow::init() {
    return initSDL("You Win!", winW, winH);
}

// Draws a dark green background with a lighter grid overlay for a retro terminal look.
void FinishWindow::drawBackground() {
    SDL_SetRenderDrawColor(renderer, 10, 20, 10, 255);
    SDL_FRect full = { 0, 0, (float)winW, (float)winH };    // Skills Audit Group B: Records (SDL_FRect struct used to describe a rectangle)
    SDL_RenderFillRect(renderer, &full);

    // Draw vertical and horizontal lines every 30 pixels to form the grid.
    SDL_SetRenderDrawColor(renderer, 20, 40, 20, 255);
    for (int x = 0; x < winW; x += 30) SDL_RenderLine(renderer, x, 0, x, winH);    // Skills Audit Group B: Simple user defined algorithms (grid line spacing calculation)
    for (int y = 0; y < winH; y += 30) SDL_RenderLine(renderer, 0, y, winW, y);
}

// Renders the "YOU ESCAPED!" heading twice — a dark shadow offset by 2 px, then the bright text on top.
void FinishWindow::drawTitle() {
    drawTextCentredInWindow(renderer, "YOU ESCAPED!", winW, 62.0f, 6.0f, 20,  20,  20 );
    drawTextCentredInWindow(renderer, "YOU ESCAPED!", winW, 60.0f, 6.0f, 100, 255, 120);    // Skills Audit Group B: Simple user defined algorithms (drop-shadow by drawing text twice with offset)
}

// Formats elapsedSeconds as mm:ss.cs and draws it below a "TIME" label.
void FinishWindow::drawTime() {
    int mins = static_cast<int>(elapsedSeconds) / 60;    // Skills Audit Group B: Simple scientific/mathematical model (time unit decomposition)
    int secs = static_cast<int>(elapsedSeconds) % 60;
    int ms   = static_cast<int>((elapsedSeconds - static_cast<int>(elapsedSeconds)) * 100);

    // Build the formatted time string using setfill and setw for zero-padding.
    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << mins << ":"
        << std::setw(2) << secs << "."
        << std::setw(2) << ms;

    drawTextCentredInWindow(renderer, "TIME",    winW, 170.0f, 3.0f, 120, 120, 120);
    drawTextCentredInWindow(renderer, oss.str(), winW, 210.0f, 6.0f, 255, 220, 80 );
}

void FinishWindow::run() {
    // Define the EXIT button rectangle, centred horizontally near the bottom of the window.
    Rect btnExit = { (winW - 260.0f) / 2.0f, 330.0f, 260.0f, 46.0f };    // Skills Audit Group B: Records (Rect struct holding button position and size)
    bool hovered = false;

    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            // Closing the window or pressing Escape exits the whole process.
            if (event.type == SDL_EVENT_QUIT) {
                cleanup();
                exit(0);
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                // Update hover state so the button brightens when the mouse is over it.
                hovered = pointInRect(event.motion.x, event.motion.y, btnExit);    // Skills Audit Group B: Simple user defined algorithms (point-in-rect hit testing)
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                       event.button.button == SDL_BUTTON_LEFT) {
                if (pointInRect(event.button.x, event.button.y, btnExit)) {
                    cleanup();
                    exit(0);
                }
            } else if (event.type == SDL_EVENT_KEY_DOWN &&
                       event.key.key == SDLK_ESCAPE) {
                cleanup();
                exit(0);
            }
        }

        // Redraw the full screen each frame.
        drawBackground();
        drawTitle();
        drawTime();
        drawButton(renderer, btnExit, 130, 40, 40, hovered, "EXIT");

        presentFrame();
    }
}
