#include "OptionsWindow.h"
#include "UIHelpers.h"

OptionsWindow::OptionsWindow(GameOptions& opts) : options(opts) {
    winW = 600;
    winH = 420;
}

bool OptionsWindow::init() {
    if (!initSDL("Options", winW, winH)) return false;
    layout();
    return true;
}

void OptionsWindow::updateButtonLabels() {
    btnAlgorithm.label  = (options.algorithm == MazeAlgorithm::RECURSIVE_DIVISION)
                          ? "Algorithm: Recursive" : "Algorithm: Depth First";
    btnToggleMap.label  = options.showMap  ? "Map Window: ON"  : "Map Window: OFF";
    btnToggleRays.label = options.showRays ? "Debug Rays: ON"  : "Debug Rays: OFF";
}

void OptionsWindow::layout() {
    float cx   = winW / 2.0f;
    float btnW = 320.0f;
    float btnH = 36.0f;
    float gap  = 6.0f;
    float top  = 140.0f;

    tbSeed        = { { cx - btnW/2, top,                              btnW, 36   }, "Seed (0 = random)", "", false, 10 };
    btnAlgorithm  = { { cx - btnW/2, top + 36 + gap,                   btnW, btnH }, "", 60,  80,  130 };
    btnToggleMap  = { { cx - btnW/2, top + 36 + (btnH + gap),          btnW, btnH }, "", 60,  100, 80  };
    btnToggleRays = { { cx - btnW/2, top + 36 + (btnH + gap) * 2,      btnW, btnH }, "", 80,  60,  100 };
    btnBack       = { { cx - btnW/2, top + 36 + (btnH + gap) * 3 + 6,  btnW, btnH }, "BACK", 100, 60,  60  };

    if (options.seed != 0)
        tbSeed.value = std::to_string(options.seed);

    updateButtonLabels();
}

void OptionsWindow::drawBackground() {
    SDL_SetRenderDrawColor(renderer, 18, 18, 28, 255);
    SDL_FRect full = { 0, 0, (float)winW, (float)winH };
    SDL_RenderFillRect(renderer, &full);

    SDL_SetRenderDrawColor(renderer, 30, 30, 45, 255);
    for (int x = 0; x < winW; x += 30) SDL_RenderLine(renderer, x, 0, x, winH);
    for (int y = 0; y < winH; y += 30) SDL_RenderLine(renderer, 0, y, winW, y);
}

void OptionsWindow::drawTitle() {
    drawTextCentredInWindow(renderer, "OPTIONS", winW, 52.0f, 3.0f, 20,  20,  20 );
    drawTextCentredInWindow(renderer, "OPTIONS", winW, 50.0f, 3.0f, 80,  160, 255);
}

void OptionsWindow::run() {
    SDL_StartTextInput(window);

    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                options.seed = tbSeed.getIntValue();
                SDL_StopTextInput(window);
                cleanup();
                return;
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                float mx = event.motion.x, my = event.motion.y;
                btnBack.hovered       = btnBack.contains(mx, my);
                btnAlgorithm.hovered  = btnAlgorithm.contains(mx, my);
                btnToggleMap.hovered  = btnToggleMap.contains(mx, my);
                btnToggleRays.hovered = btnToggleRays.contains(mx, my);
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                       event.button.button == SDL_BUTTON_LEFT) {
                float mx = event.button.x, my = event.button.y;

                if (btnBack.contains(mx, my)) {
                    options.seed = tbSeed.getIntValue();
                    SDL_StopTextInput(window);
                    cleanup();
                    return;
                }
                if (btnAlgorithm.contains(mx, my)) {
                    options.algorithm = (options.algorithm == MazeAlgorithm::RECURSIVE_DIVISION)
                                        ? MazeAlgorithm::DEPTH_FIRST
                                        : MazeAlgorithm::RECURSIVE_DIVISION;
                    updateButtonLabels();
                }
                if (btnToggleMap.contains(mx, my)) {
                    options.showMap = !options.showMap;
                    updateButtonLabels();
                }
                if (btnToggleRays.contains(mx, my)) {
                    options.showRays = !options.showRays;
                    updateButtonLabels();
                }
                tbSeed.focused = tbSeed.contains(mx, my);

            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    options.seed = tbSeed.getIntValue();
                    SDL_StopTextInput(window);
                    cleanup();
                    return;
                }
                if (event.key.key == SDLK_RETURN) tbSeed.focused = false;
                tbSeed.handleKey(event.key.key, "");
            } else if (event.type == SDL_EVENT_TEXT_INPUT) {
                tbSeed.handleTextInput(event.text.text);
            }
        }

        drawBackground();
        drawTitle();
        tbSeed.render(renderer);
        btnAlgorithm.render(renderer);
        btnToggleMap.render(renderer);
        btnToggleRays.render(renderer);
        btnBack.render(renderer);

        presentFrame();
    }
}
