#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "BaseWindow.h"
#include "Config.h"
#include "MenuWindow.h"

class OptionsWindow : public BaseWindow {
private:
    GameOptions& options;

    Button  btnBack;
    Button  btnAlgorithm;
    Button  btnToggleMap;
    Button  btnToggleRays;
    TextBox tbSeed;

    void layout();
    void drawBackground();
    void drawTitle();
    void updateButtonLabels();

public:
    explicit OptionsWindow(GameOptions& opts);
    bool init();
    void run();
};
