#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include "BaseWindow.h"
#include "RayHit.h"
#include "Player.h"
#include "Texture.h"

// Handles all pixel-level 3-D rendering: floor, ceiling and textured walls.    // Skills Audit Group A: Complex scientific/mathematical/robotics/control/business model
class GameView {
private:
    // Streaming SDL texture used as the pixel write buffer each frame.
    SDL_Texture* pixelBuffer = nullptr;
    int bufferWidth  = 0;
    int bufferHeight = 0;

    // Writes a single ARGB pixel into the locked texture buffer.
    void setPixel(uint32_t* pixels, int pitch, int x, int y,
                  uint8_t r, uint8_t g, uint8_t b);

    // Renders the floor and mirrored ceiling using raycasting-derived UV coordinates.
    void drawFloorAndCeiling(uint32_t* pixels, int pitch,
                             const Player& player,
                             int screenW, int screenH,
                             const std::vector<Texture>& textures);

    // Renders each vertical wall slice using the ray hit data and wall textures.
    void drawWalls(uint32_t* pixels, int pitch,
                   const std::vector<RayHit>& rayResults,
                   int screenW, int screenH,
                   const std::vector<Texture>& textures);

    // Calls drawFloorAndCeiling then drawWalls to produce a complete frame.
    void drawScene(uint32_t* pixels, int pitch,
                   const Player& player,
                   const std::vector<RayHit>& rayResults,
                   int screenW, int screenH,
                   const std::vector<Texture>& textures);

public:
    // Creates the streaming pixel texture at the given resolution.
    bool init(SDL_Renderer* renderer, int width, int height);
    // Destroys the pixel texture.
    void destroy();
    // Locks the texture, draws the full scene into it, then uploads and renders.
    void render(SDL_Renderer* renderer,
                const Player& player,
                const std::vector<RayHit>& rayResults,
                int screenW, int screenH,
                const std::vector<Texture>& textures);
};

// SDL window that owns a GameView and drives the main game loop.    // Skills Audit Group A: Complex user-defined use of OOP model (inheritance)
class GameWindow : public BaseWindow {
private:
    GameView gameView;
    bool     running = false;
    int      FOV;

    void cleanup();
    // Reads keyboard state and SDL quit events; updates the player accordingly.
    void handleEvents(Player& player);

public:
    GameWindow(int width = 1280, int height = 720, int fov = 120);
    ~GameWindow();

    bool isRunning();
    bool init();
    // Sets the running flag to true, starting the game loop.
    void initRun();
    // Processes input, clears the renderer, draws the scene and presents the frame.
    void update(Player& player, const std::vector<RayHit>& rayResults,
                const std::vector<Texture>& textures);
};
