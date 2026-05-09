#include "OptionsWindow.h"
#include "UIHelpers.h"

// Store a reference to the caller's GameOptions so edits persist after the window closes.
OptionsWindow::OptionsWindow(GameOptions& opts) : options(opts) {    // Skills Audit Group B: Records (GameOptions struct passed by reference to preserve state)
    winW = 600;
    winH = 420;
}

bool OptionsWindow::init() {
    if (!initSDL("Options", winW, winH)) return false;
    layout();
    return true;
}

// Refreshes all toggle button labels to reflect the current state of options.
void OptionsWindow::updateButtonLabels() {
    btnAlgorithm.label  = (options.algorithm == MazeAlgorithm::RECURSIVE_DIVISION)    // Skills Audit Group B: Simple user defined algorithms (conditional label selection)
                          ? "Algorithm: Recursive" : "Algorithm: Depth First";
    btnToggleMap.label  = options.showMap  ? "Map Window: ON"  : "Map Window: OFF";
    btnToggleRays.label = options.showRays ? "Debug Rays: ON"  : "Debug Rays: OFF";
}

// Positions all controls relative to the horizontal centre of the window.
void OptionsWindow::layout() {
    float cx   = winW / 2.0f;    // Skills Audit Group B: Simple scientific/mathematical model (horizontal centre calculation)
    float btnW = 320.0f;
    float btnH = 36.0f;
    float gap  = 6.0f;
    float top  = 140.0f;

    tbSeed        = { { cx - btnW/2, top,                              btnW, 36   }, "Seed (0 = random)", "", false, 10 };    // Skills Audit Group B: Records (TextBox struct initialised with position, label and constraints)
    btnAlgorithm  = { { cx - btnW/2, top + 36 + gap,                   btnW, btnH }, "", 60,  80,  130 };
    btnToggleMap  = { { cx - btnW/2, top + 36 + (btnH + gap),          btnW, btnH }, "", 60,  100, 80  };
    btnToggleRays = { { cx - btnW/2, top + 36 + (btnH + gap) * 2,      btnW, btnH }, "", 80,  60,  100 };
    btnBack       = { { cx - btnW/2, top + 36 + (btnH + gap) * 3 + 6,  btnW, btnH }, "BACK", 100, 60,  60  };

    // Pre-fill the seed box if a non-random seed was already set.
    if (options.seed != 0)
        tbSeed.value = std::to_string(options.seed);    // Skills Audit Group B: Simple user defined algorithms (integer-to-string conversion for display)

    updateButtonLabels();
}

// Same dark-blue grid background as the main menu for visual consistency.
void OptionsWindow::drawBackground() {
    SDL_SetRenderDrawColor(renderer, 18, 18, 28, 255);
    SDL_FRect full = { 0, 0, (float)winW, (float)winH };    // Skills Audit Group B: Records (SDL_FRect struct used as background fill region)
    SDL_RenderFillRect(renderer, &full);

    SDL_SetRenderDrawColor(renderer, 30, 30, 45, 255);
    for (int x = 0; x < winW; x += 30) SDL_RenderLine(renderer, x, 0, x, winH);    // Skills Audit Group B: Simple user defined algorithms (evenly spaced grid lines)
    for (int y = 0; y < winH; y += 30) SDL_RenderLine(renderer, 0, y, winW, y);
}

// Draws "OPTIONS" with a dark drop-shadow offset by 2 pixels.
void OptionsWindow::drawTitle() {
    drawTextCentredInWindow(renderer, "OPTIONS", winW, 52.0f, 3.0f, 20,  20,  20 );    // Skills Audit Group B: Simple user defined algorithms (drop-shadow by drawing text twice with offset)
    drawTextCentredInWindow(renderer, "OPTIONS", winW, 50.0f, 3.0f, 80,  160, 255);
}

void OptionsWindow::run() {
    // Enable SDL text input so SDL_EVENT_TEXT_INPUT events are generated for the seed box.
    SDL_StartTextInput(window);

    while (true) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                // Save the seed value before exiting.
                options.seed = tbSeed.getIntValue();    // Skills Audit Group B: Records (writing parsed value back into GameOptions struct)
                SDL_StopTextInput(window);
                cleanup();
                return;
            } else if (event.type == SDL_EVENT_MOUSE_MOTION) {
                // Update button hover states on mouse movement.
                float mx = event.motion.x, my = event.motion.y;
                btnBack.hovered       = btnBack.contains(mx, my);    // Skills Audit Group B: Simple user defined algorithms (point-in-rect per button)
                btnAlgorithm.hovered  = btnAlgorithm.contains(mx, my);
                btnToggleMap.hovered  = btnToggleMap.contains(mx, my);
                btnToggleRays.hovered = btnToggleRays.contains(mx, my);
            } else if (event.type == SDL_EVENT_MOUSE_BUTTON_DOWN &&
                       event.button.button == SDL_BUTTON_LEFT) {
                float mx = event.button.x, my = event.button.y;

                // BACK saves the seed and returns to the menu.
                if (btnBack.contains(mx, my)) {
                    options.seed = tbSeed.getIntValue();    // Skills Audit Group B: Records (persisting parsed seed back into GameOptions)
                    SDL_StopTextInput(window);
                    cleanup();
                    return;
                }
                // Toggle the maze algorithm between the two available options.
                if (btnAlgorithm.contains(mx, my)) {
                    options.algorithm = (options.algorithm == MazeAlgorithm::RECURSIVE_DIVISION)    // Skills Audit Group B: Simple user defined algorithms (two-state toggle)
                                        ? MazeAlgorithm::DEPTH_FIRST
                                        : MazeAlgorithm::RECURSIVE_DIVISION;
                    updateButtonLabels();
                }
                if (btnToggleMap.contains(mx, my)) {
                    options.showMap = !options.showMap;    // Skills Audit Group B: Simple user defined algorithms (boolean toggle)
                    updateButtonLabels();
                }
                if (btnToggleRays.contains(mx, my)) {
                    options.showRays = !options.showRays;    // Skills Audit Group B: Simple user defined algorithms (boolean toggle)
                    updateButtonLabels();
                }
                // Clicking anywhere outside the seed box removes focus.
                tbSeed.focused = tbSeed.contains(mx, my);

            } else if (event.type == SDL_EVENT_KEY_DOWN) {
                if (event.key.key == SDLK_ESCAPE) {
                    options.seed = tbSeed.getIntValue();    // Skills Audit Group B: Records (persisting seed before closing)
                    SDL_StopTextInput(window);
                    cleanup();
                    return;
                }
                // Return confirms the seed entry and removes focus from the text box.
                if (event.key.key == SDLK_RETURN) tbSeed.focused = false;
                tbSeed.handleKey(event.key.key, "");
            } else if (event.type == SDL_EVENT_TEXT_INPUT) {
                // Forward typed characters to the seed text box.
                tbSeed.handleTextInput(event.text.text);    // Skills Audit Group B: Simple user defined algorithms (digit-only text input filtering)
            }
        }

        // Redraw all controls each frame.
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
