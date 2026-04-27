#pragma once
#include <SDL3/SDL.h>
#include "BaseWindow.h"

class FinishWindow : public BaseWindow {
private:
    double elapsedSeconds;

    void drawBackground();
    void drawTitle();
    void drawTime();
    void layout();

public:
    FinishWindow(double elapsed);
    bool init();
    void run();
};
