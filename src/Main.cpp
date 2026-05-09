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

// Checks whether the player is close enough to the exit tile to trigger the win screen.
static void checkWinCondition(const Player& player, time_t startTime) {
    // The exit is placed at the far bottom-right corner of the map.
    float dx = player.getX() - (MAP_WIDTH  - 0.5f);    // Skills Audit Group B: Simple scientific/mathematical model (offset to tile centre)
    float dy = player.getY() - (MAP_HEIGHT - 1.5f);
    // Use squared distance to avoid a sqrt; 1.5 tiles radius counts as reaching the exit.
    if (dx * dx + dy * dy < 1.5f * 1.5f) {    // Skills Audit Group B: Simple scientific/mathematical model (squared distance proximity check)
        double elapsed = std::difftime(std::time(nullptr), startTime);    // Skills Audit Group B: Simple user defined algorithms (wall-clock elapsed time)
        FinishWindow win(elapsed);    // Skills Audit Group B: Generation of objects based on simple OOP model
        if (win.init()) win.run();
        SDL_Quit();
        exit(0);
    }
}

int main() {
    // Initialise SDL video subsystem; abort if it fails.
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        std::cerr << "SDL_Init failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    // Show the main menu and wait for the user to choose START or EXIT.
    MenuWindow menu;    // Skills Audit Group B: Generation of objects based on simple OOP model
    if (!menu.init()) { SDL_Quit(); return 1; }

    MenuResult menuResult = menu.run();    // Skills Audit Group B: Records (MenuResult struct carries action and options together)
    if (menuResult.action == MenuAction::EXIT) { SDL_Quit(); return 0; }

    // Extract the options chosen in the menu (seed, algorithm, debug flags).
    const GameOptions& opts = menuResult.options;    // Skills Audit Group B: Records (GameOptions struct groups related settings)
    std::cout << "Starting game with seed: " << opts.seed
              << ", algorithm: " << (opts.algorithm == MazeAlgorithm::RECURSIVE_DIVISION
                                     ? "Recursive Division" : "Depth First") << std::endl;

    // Generate the maze using whichever algorithm the player selected.    // Skills Audit Group A: Complex user-defined algorithms (dynamic generation of objects based on complex OOP model)
    std::vector<std::vector<int>> mazeData;    // Skills Audit Group B: Multi-dimensional arrays (2-D maze grid storage)
    if (opts.algorithm == MazeAlgorithm::RECURSIVE_DIVISION) {
        RecursiveDivisionMazeGenerator rdGen(MAP_WIDTH, MAP_HEIGHT, opts.seed);    // Skills Audit Group B: Generation of objects based on simple OOP model
        rdGen.generateMaze();
        mazeData = rdGen.getMaze();
    } else {
        DepthFirstMazeGenerator dfGen(MAP_WIDTH, MAP_HEIGHT, opts.seed);    // Skills Audit Group B: Generation of objects based on simple OOP model
        dfGen.generateMaze();
        mazeData = dfGen.getMaze();
    }

    // Wrap the generated grid in a WorldMap for spatial queries and pathfinding.
    WorldMap worldMap(mazeData, MAP_HEIGHT, MAP_WIDTH);    // Skills Audit Group B: Generation of objects based on simple OOP model

    // Initialise the player at the starting tile with the configured speed and FOV.
    Player player(PLAYER_START_X, PLAYER_START_Y, PLAYER_START_ANGLE,
                  PLAYER_FOV, PLAYER_ROTATE_SPEED, PLAYER_MOVE_SPEED);    // Skills Audit Group B: Generation of objects based on simple OOP model
    player.setWorldMap(&worldMap);

    // Create and open the main 3-D game window.
    GameWindow gameView(SCREEN_WIDTH, SCREEN_HEIGHT, PLAYER_FOV);    // Skills Audit Group B: Generation of objects based on simple OOP model
    if (!gameView.init()) { SDL_Quit(); return 2; }
    gameView.initRun();

    // Optionally open the overhead minimap window.
    MapWindow* mapView = nullptr;
    if (opts.showMap) {
        mapView = new MapWindow((MAP_HEIGHT * 9) - 1, (MAP_WIDTH * 9) - 1);    // Skills Audit Group B: Generation of objects based on simple OOP model
        if (!mapView->init()) { delete mapView; mapView = nullptr; }
        else mapView->initRun(worldMap, opts.showRays);
    }

    // Initialise the raycaster with the world map and set the maximum ray travel distance.
    Raycaster raycaster(worldMap);    // Skills Audit Group B: Generation of objects based on simple OOP model
    raycaster.setMaxDistance(RAY_MAX_DISTANCE);

    // Create both wall textures procedurally at start-up.
    std::vector<Texture> textures = { makeBrickTexture(), makeStoneTexture() };    // Skills Audit Group B: Simple user defined algorithms (procedural texture generation at startup)

    // Record the start time so elapsed seconds can be shown on the win screen.
    const time_t startTime = std::time(nullptr);    // Skills Audit Group B: Simple user defined algorithms (recording game start timestamp)

    // Main game loop: cast rays, render, update the minimap, and check for win condition.
    while (gameView.isRunning()) {
        std::vector<RayHit> rayResults = raycaster.castAllRays(player, SCREEN_WIDTH);    // Skills Audit Group A: Complex scientific/mathematical model (DDA raycasting for all screen columns)
        gameView.update(player, rayResults, textures);
        if (mapView && mapView->isRunning())
            mapView->update(player, rayResults);
        checkWinCondition(player, startTime);
    }

    delete mapView;
    SDL_Quit();
    return 0;
}
