#include <ctime>
#include <SDL3/SDL.h>
#include <iostream>
#include "Config.h"
#include "Raycaster.h"
#include "DepthFirstMazeGenerator.h"
#include "RecursiveDivisionMazeGenerator.h"
#include "MapWindow.h"
#include "GameWindow.h"
#include "MenuWindow.h"
#include "FinishWindow.h"

static void checkWinCondition(const Player& player, time_t startTime) {
    float dx = player.getX() - (MAP_WIDTH  - 0.5f);
    float dy = player.getY() - (MAP_HEIGHT - 1.5f);
    if (dx * dx + dy * dy < 1.5f * 1.5f) {
        double elapsed = std::difftime(std::time(nullptr), startTime);
        FinishWindow win(elapsed);
        if (win.init()) win.run();
        SDL_Quit();
        exit(0);
    }
}

int main() {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    MenuWindow menu;
    if (!menu.init()) { SDL_Quit(); return 1; }

    MenuResult menuResult = menu.run();
    if (menuResult.action == MenuAction::EXIT) { SDL_Quit(); return 0; }

    const GameOptions& opts = menuResult.options;
    std::cout << "Starting game with seed: " << opts.seed
              << ", algorithm: " << (opts.algorithm == MazeAlgorithm::RECURSIVE_DIVISION
                                     ? "Recursive Division" : "Depth First") << std::endl;

    std::vector<std::vector<int>> mazeData;
    if (opts.algorithm == MazeAlgorithm::RECURSIVE_DIVISION) {
        RecursiveDivisionMazeGenerator rdGen(MAP_WIDTH, MAP_HEIGHT, opts.seed);
        rdGen.generateMaze();
        mazeData = rdGen.getMaze();
    } else {
        DepthFirstMazeGenerator dfGen(MAP_WIDTH, MAP_HEIGHT, opts.seed);
        dfGen.generateMaze();
        mazeData = dfGen.getMaze();
    }

    WorldMap worldMap(mazeData, MAP_HEIGHT, MAP_WIDTH);

    Player player(PLAYER_START_X, PLAYER_START_Y, PLAYER_START_ANGLE,
                  PLAYER_FOV, PLAYER_ROTATE_SPEED, PLAYER_MOVE_SPEED);
    player.setWorldMap(&worldMap);

    GameWindow gameView(SCREEN_WIDTH, SCREEN_HEIGHT, PLAYER_FOV);
    if (!gameView.init()) { SDL_Quit(); return 2; }
    gameView.initRun();

    MapWindow* mapView = nullptr;
    if (opts.showMap) {
        mapView = new MapWindow((MAP_HEIGHT * 9) - 1, (MAP_WIDTH * 9) - 1);
        if (!mapView->init()) { delete mapView; mapView = nullptr; }
        else mapView->initRun(worldMap, opts.showRays);
    }

    Raycaster raycaster(worldMap);
    raycaster.setMaxDistance(RAY_MAX_DISTANCE);

    std::vector<Texture> textures = { makeBrickTexture(), makeStoneTexture() };

    const time_t startTime = std::time(nullptr);
    while (gameView.isRunning()) {
        std::vector<RayHit> rayResults = raycaster.castAllRays(player, SCREEN_WIDTH);
        gameView.update(player, rayResults, textures);
        if (mapView && mapView->isRunning())
            mapView->update(player, rayResults);
        checkWinCondition(player, startTime);
    }

    delete mapView;
    SDL_Quit();
    return 0;
}
