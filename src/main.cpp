#pragma once

#include <ctime>

#include "raycaster.cpp"
#include "mapWindow.cpp"
#include "gameWindow.cpp"
#include "menuWindow.cpp"

#include "depthFirstMazeGenerator.h"
#include "recursiveDivisionMazeGenerator.h"


int gameWin(time_t startTime) {
    double elapsed = std::difftime(std::time(nullptr), startTime);
    std::cout << "Finished maze in " << elapsed << " Seconds" << std::endl;
    exit(0);
}

int main() {

    // ── Menu ─────────────────────────────────────────────────────────────────
    MenuWindow menu;
    if (!menu.init()) return 1;

    MenuResult menuResult = menu.run();

    if (menuResult.action == MenuAction::EXIT) {
        return 0;
    }

    if (menuResult.action == MenuAction::OPTIONS) {
        // Options not yet implemented — fall through to game for now
        std::cout << "Options not yet implemented, starting game." << std::endl;
    }

    int seed = menuResult.seed;
    std::cout << "Starting game with seed: " << seed << std::endl;
    // ─────────────────────────────────────────────────────────────────────────


    // ── Maze Init ─────────────────────────────────────────────────────────────
    int height = 63;
    int width  = 63;

    DepthFirstMazeGenerator dfGen(width, height, seed);
    dfGen.generateMaze();

    RecursiveDivisionMazeGenerator rdGen(width, height, seed);
    rdGen.generateMaze();

    std::vector<std::vector<int>> map = rdGen.getMaze();
    // ─────────────────────────────────────────────────────────────────────────


    // ── Player Init ───────────────────────────────────────────────────────────
    float playerX     = 1.5f;
    float playerY     = 1.5f;
    float playerAngle = 0.0f;
    float FOV         = 90.0f;
    float screenWidth  = 1280.0f;
    float screenHeight = 720.0f;

    Player player(playerX, playerY, playerAngle, FOV);
    // ─────────────────────────────────────────────────────────────────────────


    // ── WorldMap + MiniMap Init ───────────────────────────────────────────────
    WorldMap worldMap(map, height, width);

    MapWindow mapView((height * 9) - 1, (width * 9) - 1);
    player.setWorldMap(&worldMap);

    if (!mapView.init()) return 1;
    mapView.initRun(worldMap);
    // ─────────────────────────────────────────────────────────────────────────


    // ── GameWindow Init ───────────────────────────────────────────────────────
    GameWindow gameView(screenWidth, screenHeight, FOV);
    if (!gameView.init()) return 2;
    gameView.initRun();
    // ─────────────────────────────────────────────────────────────────────────


    // ── Raycaster Init ────────────────────────────────────────────────────────
    Raycaster raycaster(worldMap);
    raycaster.setMaxDistance(20.0f);
    // ─────────────────────────────────────────────────────────────────────────


    // ── Textures ──────────────────────────────────────────────────────────────
    std::vector<Texture> textures;
    textures.push_back(makeBrickTexture());
    textures.push_back(makeStoneTexture());
    // ─────────────────────────────────────────────────────────────────────────


    // ── Main Loop ─────────────────────────────────────────────────────────────
    time_t startTime = std::time(nullptr);

    while (mapView.isRunning() || gameView.isRunning()) {
        std::vector<RayHit> rayResults = raycaster.castAllRays(player, screenWidth);

        gameView.update(player, rayResults, textures);
        mapView.update(player, rayResults);

        // Win condition
        float dx = player.getX() - (width  - 0.5f);
        float dy = player.getY() - (height - 1.5f);
        if (dx * dx + dy * dy < 1.5f * 1.5f) {
            gameWin(startTime);
        }
    }

    return 0;
}
