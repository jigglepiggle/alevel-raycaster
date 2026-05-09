#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "BaseWindow.h"
#include "Config.h"

// Possible outcomes when the menu event loop exits.
enum class MenuAction { START, EXIT };

// Returned by MenuWindow::run() to carry both the chosen action and configured options.
struct MenuResult {
    MenuAction  action;
    GameOptions options;
};

// A clickable UI button with a coloured background and centred label.    // Skills Audit Group A: Generation of objects based on simple OOP model
struct Button {
    Rect        bounds;
    std::string label;
    uint8_t     colR, colG, colB;   // Base fill colour; brightened when hovered.
    bool        hovered = false;

    void render(SDL_Renderer* renderer) const;
    // Returns true if the given mouse coordinate falls within the button bounds.
    bool contains(float mx, float my) const;
};

// A numeric text-entry field that only accepts digit characters.
struct TextBox {
    Rect        bounds;
    std::string label;    // Helper label drawn above the box.
    std::string value;    // Current text content.
    bool        focused = false;
    int         maxLen  = 12;

    void render(SDL_Renderer* renderer) const;
    bool contains(float mx, float my) const;
    // Handles Backspace to delete the last character.
    void handleKey(SDL_Keycode key, const std::string& input);
    // Appends digit characters from SDL text-input events up to maxLen.
    void handleTextInput(const std::string& inputText);
    // Parses and returns the current value as an integer; returns 0 if empty or invalid.
    int  getIntValue() const;
};

// Main menu window: shows the game title and START / OPTIONS / EXIT buttons.    // Skills Audit Group A: Complex user-defined use of OOP model (inheritance)
class MenuWindow : public BaseWindow {
private:
    Button      btnStart;
    Button      btnOptions;
    Button      btnExit;
    // Holds the current game settings, updated when the user visits OptionsWindow.
    GameOptions options;

    void drawBackground();
    void drawTitle();
    // Positions all buttons relative to the window centre.
    void layout();

public:
    MenuWindow();
    bool       init();
    // Runs the menu event loop and returns the chosen action plus settings.
    MenuResult run();
};
