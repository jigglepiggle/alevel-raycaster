#include <ctime>
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

    const int seed = menuResult.seed;
    std::cout << "Starting game with seed: " << seed << std::endl;

    RecursiveDivisionMazeGenerator rdGen(MAP_WIDTH, MAP_HEIGHT, seed);
    rdGen.generateMaze();
    [[maybe_unused]] DepthFirstMazeGenerator dfGen(MAP_WIDTH, MAP_HEIGHT, seed);
    dfGen.generateMaze();

    WorldMap worldMap(rdGen.getMaze(), MAP_HEIGHT, MAP_WIDTH);

    Player player(PLAYER_START_X, PLAYER_START_Y, PLAYER_START_ANGLE, PLAYER_FOV, PLAYER_ROTATE_SPEED, PLAYER_MOVE_SPEED);
    player.setWorldMap(&worldMap);

    MapWindow mapView((MAP_HEIGHT * 9) - 1, (MAP_WIDTH * 9) - 1);
    if (!mapView.init()) return 1;
    mapView.initRun(worldMap);

    GameWindow gameView(SCREEN_WIDTH, SCREEN_HEIGHT, PLAYER_FOV);
    if (!gameView.init()) return 2;
    gameView.initRun();

    Raycaster raycaster(worldMap);
    raycaster.setMaxDistance(RAY_MAX_DISTANCE);

    std::vector<Texture> textures = { makeBrickTexture(), makeStoneTexture() };

    const time_t startTime = std::time(nullptr);
    while (mapView.isRunning() || gameView.isRunning()) {
        std::vector<RayHit> rayResults = raycaster.castAllRays(player, SCREEN_WIDTH);
        gameView.update(player, rayResults, textures);
        mapView.update(player, rayResults);
        checkWinCondition(player, startTime);
    }

    return 0;
}
