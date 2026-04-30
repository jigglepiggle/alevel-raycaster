#include "FinishWindow.h"
#include "UIHelpers.h"
#include <iomanip>
#include <sstream>

FinishWindow::FinishWindow(double elapsed) : elapsedSeconds(elapsed) {
    winW = 600;
    winH = 420;
}

bool FinishWindow::init() {
    return initSDL("You Win!", winW, winH);
}

void FinishWindow::drawBackground() {
    SDL_SetRenderDrawColor(renderer, 10, 20, 10, 255);
    SDL_FRect full = { 0, 0, (float)winW, (float)winH };
    SDL_RenderFillRect(renderer, &full);

    SDL_SetRenderDrawColor(renderer, 20, 40, 20, 255);
    for (int x = 0; x < winW; x += 30) SDL_RenderLine(renderer, x, 0, x, winH);
    for (int y = 0; y < winH; y += 30) SDL_RenderLine(renderer, 0, y, winW, y);
}

void FinishWindow::drawTitle() {
    drawTextCentredInWindow(renderer, "YOU ESCAPED!", winW, 62.0f, 6.0f, 20,  20,  20 );
    drawTextCentredInWindow(renderer, "YOU ESCAPED!", winW, 60.0f, 6.0f, 100, 255, 120);
}

void FinishWindow::drawTime() {
    int mins = static_cast<int>(elapsedSeconds) / 60;
    int secs = static_cast<int>(elapsedSeconds) % 60;
    int ms   = static_cast<int>((elapsedSeconds - static_cast<int>(elapsedSeconds)) * 100);

    std::ostringstream oss;
    oss << std::setfill('0')
        << std::setw(2) << mins << ":"
        << std::setw(2) << secs << "."
        << std::setw(2) << ms;

    drawTextCentredInWindow(renderer, "TIME",    winW, 170.0f, 3.0f, 120, 120, 120);
    drawTextCentredInWindow(renderer, oss.str(), winW, 210.0f, 6.0f, 255, 220, 80 );
}

void FinishWindow::run() {
    Rect btnExit = { (winW - 260.0f) / 2.0f, 330.0f, 260.0f, 46.0f };
    bool hovered = false;

    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                cleanup();
                exit(0);
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                hovered = pointInRect(event.motion.x, event.motion.y, btnExit);
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

        drawBackground();
        drawTitle();
        drawTime();
        drawButton(renderer, btnExit, 130, 40, 40, hovered, "EXIT");

        presentFrame();
    }
}
