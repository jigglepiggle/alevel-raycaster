#pragma once
#include <SDL3/SDL.h>
#include "BaseWindow.h"

// Displayed when the player reaches the maze exit; shows elapsed time and an exit button.    // Skills Audit Group A: Complex user-defined use of OOP model (inheritance)
class FinishWindow : public BaseWindow {
private:
    // Time in seconds from game start to reaching the exit.
    double elapsedSeconds;

    // Draws the dark grid background.
    void drawBackground();
    // Draws the "YOU ESCAPED!" heading with a drop-shadow effect.
    void drawTitle();
    // Formats and draws the mm:ss.ms completion time.
    void drawTime();
    void layout();

public:
    // Constructs the window with the recorded elapsed time.
    FinishWindow(double elapsed);
    bool init();
    // Enters the event loop; exits the process when the player clicks EXIT or presses Escape.
    void run();
};
