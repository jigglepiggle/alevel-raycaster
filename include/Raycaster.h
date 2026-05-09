#pragma once
#include <vector>
#include "RayHit.h"
#include "WorldMap.h"
#include "Player.h"

// Performs DDA (Digital Differential Analysis) raycasting to determine wall intersections.    // Skills Audit Group A: Complex scientific/mathematical/robotics/control/business model
class Raycaster {
private:
    // Distinguishes which axis the DDA algorithm stepped along when it hit a wall.
    enum class WallType { HORIZONTAL, VERTICAL };

    // Rays that exceed this distance are stopped to avoid checking the full map.
    float maxRayDistance;
    // Local copy of the 2-D map grid used for wall lookups during casting.    // Skills Audit Group A: Advanced matrix operations
    std::vector<std::vector<int>> worldMap;

    // Casts one ray from (startX, startY) in the given direction and returns hit data.
    RayHit castSingleRay(float startX, float startY, 
                       float rayDirectionX, float rayDirectionY, float playerAngle);

public:
    explicit Raycaster(WorldMap worldMapObj);

    // Casts one ray per horizontal pixel column and returns the full list of hits.
    std::vector<RayHit> castAllRays(Player& player, int screenWidth);
    void setMaxDistance(float distance);
};
