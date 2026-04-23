#include <ctime>
#include "raycaster.cpp"
#include "mapWindow.cpp"
#include "gameWindow.cpp"
#include "menuWindow.cpp"
#include "depthFirstMazeGenerator.h"
#include "recursiveDivisionMazeGenerator.h"

static void checkWinCondition(const Player& player, int width, int height, time_t startTime) {
    float dx = player.getX() - (width  - 0.5f);
    float dy = player.getY() - (height - 1.5f);
    if (dx * dx + dy * dy < 1.5f * 1.5f) {
        double elapsed = std::difftime(std::time(nullptr), startTime);
        std::cout << "Finished maze in " << elapsed << " seconds" << std::endl;
        exit(0);
    }
}

int main() {
    MenuWindow menu;
    if (!menu.init()) return 1;

    MenuResult menuResult = menu.run();
    if (menuResult.action == MenuAction::EXIT) return 0;
    if (menuResult.action == MenuAction::OPTIONS)
        std::cout << "Options not yet implemented, starting game." << std::endl;

    const int seed   = menuResult.seed;
    const int width  = 63;
    const int height = 63;
    std::cout << "Starting game with seed: " << seed << std::endl;

    RecursiveDivisionMazeGenerator rdGen(width, height, seed);
    rdGen.generateMaze();
    [[maybe_unused]] DepthFirstMazeGenerator dfGen(width, height, seed);
    dfGen.generateMaze();
    std::vector<std::vector<int>> map = rdGen.getMaze();

    const float playerX     = 1.5f;
    const float playerY     = 1.5f;
    const float playerAngle = 0.0f;
    const float FOV         = 90.0f;
    const float screenWidth  = 1280.0f;
    const float screenHeight = 720.0f;

    Player player(playerX, playerY, playerAngle, FOV);
    WorldMap worldMap(map, height, width);
    player.setWorldMap(&worldMap);

    MapWindow mapView((height * 9) - 1, (width * 9) - 1);
    if (!mapView.init()) return 1;
    mapView.initRun(worldMap);

    GameWindow gameView(screenWidth, screenHeight, FOV);
    if (!gameView.init()) return 2;
    gameView.initRun();

    Raycaster raycaster(worldMap);
    raycaster.setMaxDistance(20.0f);

    std::vector<Texture> textures = { makeBrickTexture(), makeStoneTexture() };

    const time_t startTime = std::time(nullptr);
    while (mapView.isRunning() || gameView.isRunning()) {
        std::vector<RayHit> rayResults = raycaster.castAllRays(player, screenWidth);
        gameView.update(player, rayResults, textures);
        mapView.update(player, rayResults);
        checkWinCondition(player, width, height, startTime);
    }

    return 0;
}
