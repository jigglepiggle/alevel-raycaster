#pragma once

#include <ctime>

#include "raycaster.cpp"
#include "mapWindow.cpp"
#include "gameWindow.cpp"

#include "depthFirstMazeGenerator.h"
#include "recursiveDivisionMazeGenerator.h"
#include "texture.cpp"


int gameWin(time_t startTime) {
    // End timer
    double elapsed = std::difftime(std::time(nullptr), startTime);
    std::cout << "Finished maze in " << elapsed << " Seconds" <<std::endl;
    exit(0);
}

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

    std::vector<std::vector<int>> map = rdGen.getMaze();
    // End Maze Init


    // Start Player Init
    float playerX = 1.5;
    float playerY = 1.5;
    float playerAngle = 0;
    float FOV = 90;
    float screenWidth = 1280;
    float screenHeight = 720;

    Player player(playerX, playerY, playerAngle, FOV);
    // End Player Init
    
    // Start MiniMap Init
    WorldMap worldMap(map, height, width);

    std::cout << worldMap.getHeight() << std::endl;
    std::cout << worldMap.isWall(0, 0) << std::endl;

    MapWindow mapView((height*9)-1, (width*9)-1);

    player.setWorldMap(&worldMap);

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

    raycaster.setMaxDistance(12.0);
    // End Raycaster Init

    // After worldMap setup:
    std::vector<Texture> textures;
    textures.push_back(makeBrickTexture());   // wallType 1
    textures.push_back(makeStoneTexture());   // wallType 2 (optional)

    // Start Timer
    time_t startTime = std::time(nullptr);

    // Main Loop
    while (mapView.isRunning() || gameView.isRunning())
    {
        std::vector<RayHit> rayResults = raycaster.castAllRays(player, screenWidth); 

        gameView.update(player, rayResults, textures);
        mapView.update(player, rayResults);

        // Win condition check
        float dx = player.getX() - (width - 0.5);   // Final block is indexed by [width-1, height-2]
        float dy = player.getY() - (height - 1.5);  // Use 0.5 so check is based on centre of the area
        if (dx*dx + dy*dy < 1.5f * 1.5f) {
            gameWin(startTime);
        }

    }
    
    return 0;
}
