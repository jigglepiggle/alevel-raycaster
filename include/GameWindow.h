#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "BaseWindow.h"
#include "RayHit.h"
#include "Player.h"
#include "Texture.h"

class GameView {
private:
    SDL_Texture* pixelBuffer = nullptr;
    int bufferWidth  = 0;
    int bufferHeight = 0;

    void setPixel(uint32_t* pixels, int pitch, int x, int y,
                  uint8_t r, uint8_t g, uint8_t b);
    void drawScene(uint32_t* pixels, int pitch,
                   const std::vector<RayHit>& rayResults,
                   int screenW, int screenH,
                   const std::vector<Texture>& textures);

public:
    bool init(SDL_Renderer* renderer, int width, int height);
    void destroy();
    void render(SDL_Renderer* renderer,
                const std::vector<RayHit>& rayResults,
                int screenW, int screenH,
                const std::vector<Texture>& textures);
};

class GameWindow : public WindowBase {
private:
    GameView gameView;
    bool     running = false;
    int      FOV;

    void cleanup();
    void handleEvents(Player& player);

public:
    GameWindow(int width = 1280, int height = 720, int fov = 120);
    ~GameWindow();

    bool isRunning();
    bool init();
    void initRun();
    void update(Player& player, const std::vector<RayHit>& rayResults,
                const std::vector<Texture>& textures);
};
