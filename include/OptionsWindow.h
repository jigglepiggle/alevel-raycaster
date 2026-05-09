#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "BaseWindow.h"
#include "Config.h"
#include "MenuWindow.h"

// Options screen that lets the player configure seed, algorithm, and debug overlays.    // Skills Audit Group A: Complex user-defined use of OOP model (inheritance)
class OptionsWindow : public BaseWindow {
private:
    // Reference to the caller's GameOptions so changes persist after the window closes.
    GameOptions& options;

    Button  btnBack;
    Button  btnAlgorithm;    // Toggles between Depth-First and Recursive Division.
    Button  btnToggleMap;    // Toggles the minimap window.
    Button  btnToggleRays;   // Toggles the debug ray overlay.
    TextBox tbSeed;          // Numeric seed entry field.

    // Sets button positions and sizes relative to the window centre.
    void layout();
    void drawBackground();
    void drawTitle();
    // Refreshes button labels to match the current option state.
    void updateButtonLabels();

public:
    explicit OptionsWindow(GameOptions& opts);
    bool init();
    // Runs the options event loop; saves settings back through the reference on exit.
    void run();
};
