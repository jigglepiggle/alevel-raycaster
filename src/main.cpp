#pragma once

#include "raycaster.cpp"
#include "mapWindow.cpp"
#include "gameWindow.cpp"

#include "depthFirstMazeGenerator.h"
#include "recursiveDivisionMazeGenerator.h"

// Main Game Loop
int main() {

    // Start Maze Init
    int height = 63;
    int width = 63;
    int seed = 0;

    DepthFirstMazeGenerator dfGen(width, height, seed);
    dfGen.generateMaze();
    
    RecursiveDivisionMazeGenerator rdGen(width, height, seed);
    rdGen.generateMaze();

    std::vector<std::vector<int>> map = dfGen.getMaze();
    // End Maze Init


    // Start Player Init
    float playerX = 0.5;
    float playerY = 1.5;
    float playerAngle = 0;
    float FOV = 120;
    float screenWidth = 1280;
    float screenHeight = 720;

    Player player(playerX, playerY, playerAngle, FOV);
    // End Player Init

    // Start MiniMap Init
    WorldMap worldMap(map, height, width);

    std::cout << worldMap.getHeight() << std::endl;
    std::cout << worldMap.isWall(0, 0) << std::endl;

    MapWindow mapView((height*9)-1, (width*9)-1);

    if (!mapView.init()) {
        return 1;
    }

    mapView.initRun(worldMap);
    // End MiniMap Init

    // Start GameView Init
    GameWindow gameView(screenWidth, screenHeight, FOV);

    if (!gameView.init()) {
        return 2;
    }

    gameView.initRun();
    // End GameView Init

    // Start Raycaster Init
    Raycaster raycaster(worldMap);

    raycaster.setMaxDistance(6.0);
    // End Raycaster Init

    // Main Loop
    while (mapView.isRunning() || gameView.isRunning())
    {
        std::vector<RayHit> rayResults = raycaster.castAllRays(player, screenWidth); 

        gameView.update(player, rayResults);
        mapView.update(player, rayResults);

    }
    
    return 0;
}