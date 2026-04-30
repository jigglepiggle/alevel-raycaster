#pragma once
#include <SDL3/SDL.h>
#include <vector>
#include <utility>
#include "BaseWindow.h"
#include "RayHit.h"
#include "Player.h"
#include "WorldMap.h"

struct DebugState {
    bool showSolutionPath = false;
    bool showRays         = false;
};

class SolutionPath {
public:
    void render(SDL_Renderer* renderer,
                const std::vector<std::pair<int,int>>& path,
                int cellSize = 9);
};

class PlayerView {
private:
    void drawPlayer(SDL_Renderer* renderer, const Player& player);
    void drawRays(SDL_Renderer* renderer, const Player& player,
                  const std::vector<RayHit>& rayResults);
public:
    void render(SDL_Renderer* renderer, const Player& player,
                const std::vector<RayHit>& rayResults, bool showRays);
};

class TeleportCursor {
public:
    void render(SDL_Renderer* renderer, float mouseX, float mouseY, int cellSize = 9);
};

class Grid {
private:
    int squareSize;
    int borderSize;
    int cellSize;
    int width;
    int height;
    std::vector<std::vector<int>> worldMap;

public:
    Grid(int squareSize = 8, int borderSize = 1, int width = 566, int height = 566);
    void setWorldMap(const std::vector<std::vector<int>>& map);
    void render(SDL_Renderer* renderer);
};

class MapWindow : public BaseWindow {
private:
    bool           running = false;
    Grid           grid;
    PlayerView     playerView;
    SolutionPath   solutionPathView;
    TeleportCursor teleportCursor;
    DebugState     debug;
    float          mouseX    = 0.0f;
    float          mouseY    = 0.0f;
    const WorldMap* worldMap = nullptr;
    std::vector<std::pair<int,int>> solution;

    static const int CELL_SIZE = 9;

    void cleanup();
    void handleEvents(Player& player);

public:
    MapWindow(int width = 566, int height = 566);
    ~MapWindow();

    bool isRunning();
    bool init();
    void initRun(WorldMap& map, bool showRays);
    void update(Player& player, const std::vector<RayHit>& rayResults);
};
