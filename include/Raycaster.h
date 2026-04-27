#pragma once
#include <vector>
#include "RayHit.h"
#include "WorldMap.h"
#include "Player.h"

class Raycaster {
private:
    enum class WallType { HORIZONTAL, VERTICAL };

    float maxRayDistance;
    std::vector<std::vector<int>> worldMap;

    RayHit castSingleRay(float startX, float startY, 
                       float rayDirectionX, float rayDirectionY, float playerAngle);

public:
    explicit Raycaster(WorldMap worldMapObj);

    std::vector<RayHit> castAllRays(Player& player, int screenWidth);
    void setMaxDistance(float distance);
};
