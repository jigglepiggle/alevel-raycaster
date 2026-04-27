#pragma once
#include <SDL3/SDL.h>
#include <string>
#include "BaseWindow.h"

enum class MenuAction { START, OPTIONS, EXIT };

struct MenuResult {
    MenuAction action;
    int        seed;
};

struct Button {
    Rect        bounds;
    std::string label;
    uint8_t     colR, colG, colB;
    bool        hovered = false;

    void render(SDL_Renderer* renderer) const;
    bool contains(float mx, float my) const;
};

struct TextBox {
    Rect        bounds;
    std::string label;
    std::string value;
    bool        focused = false;
    int         maxLen  = 12;

    void render(SDL_Renderer* renderer) const;
    bool contains(float mx, float my) const;
    void handleKey(SDL_Keycode key, const std::string& input);
    void handleTextInput(const std::string& inputText);
    int  getIntValue() const;
};

class MenuWindow : public BaseWindow {
private:
    Button  btnStart;
    Button  btnOptions;
    Button  btnExit;
    TextBox tbSeed;

    void drawBackground();
    void drawTitle();
    void layout();

public:
    MenuWindow();
    bool       init();
    MenuResult run();
};
