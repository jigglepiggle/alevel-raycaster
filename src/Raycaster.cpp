#include "Player.h"
#include "WorldMap.h"
#include "RayHit.h"
#include "Raycaster.h"
#include <math.h>

// Copy the world map grid for wall lookups during ray traversal.
Raycaster::Raycaster(WorldMap worldMapObj) { worldMap = worldMapObj.getMap(); }    // Skills Audit Group B: Multi-dimensional arrays (storing a copy of the 2-D map grid)

RayHit Raycaster::castSingleRay(float startX, float startY, float rayDirectionX, float rayDirectionY, float playerAngle) {
    // Normalise the direction vector so the DDA step calculations are correct.    // Skills Audit Group A: Complex scientific/mathematical model (DDA raycasting algorithm)
    float rayLength = sqrt(rayDirectionX * rayDirectionX + rayDirectionY * rayDirectionY);    // Skills Audit Group B: Simple scientific/mathematical model (vector magnitude)
    rayDirectionX /= rayLength;    // Skills Audit Group B: Simple scientific/mathematical model (vector normalisation)
    rayDirectionY /= rayLength;

    // Start in the map tile that contains the player's position.
    int currentMapX = static_cast<int>(floor(startX));    // Skills Audit Group B: Simple scientific/mathematical model (world position floored to tile index)
    int currentMapY = static_cast<int>(floor(startY));

    // deltaDist is the distance the ray travels between consecutive X or Y grid crossings.
    float deltaDistX = abs(1.0f / rayDirectionX);    // Skills Audit Group A: Complex scientific/mathematical model (DDA inter-crossing distance)
    float deltaDistY = abs(1.0f / rayDirectionY);

    int stepX, stepY;
    float sideDistX, sideDistY;

    // Set the step direction and initial side-distances based on ray direction sign.
    if (rayDirectionX < 0) {
        stepX     = -1;
        sideDistX = (startX - currentMapX) * deltaDistX;    // Skills Audit Group B: Simple scientific/mathematical model (fractional distance to first left crossing)
    } else {
        stepX     = 1;
        sideDistX = (currentMapX + 1.0f - startX) * deltaDistX;    // Skills Audit Group B: Simple scientific/mathematical model (fractional distance to first right crossing)
    }

    if (rayDirectionY < 0) {
        stepY     = -1;
        sideDistY = (startY - currentMapY) * deltaDistY;
    } else {
        stepY     = 1;
        sideDistY = (currentMapY + 1.0f - startY) * deltaDistY;
    }

    bool hitWall = false;
    WallType hitSide = WallType::VERTICAL;

    int mapHeight = static_cast<int>(worldMap.size());    // Skills Audit Group B: Multi-dimensional arrays (reading row/column extents of the 2-D grid)
    int mapWidth  = static_cast<int>(worldMap[0].size());

    // DDA loop: step along whichever axis has the nearer next crossing until a wall is hit.    // Skills Audit Group A: Stack/Queue Operations (iterative traversal replacing recursion)
    while (!hitWall) {
        if (sideDistX < sideDistY) {
            // Bail out if the ray has exceeded the maximum draw distance.
            if (sideDistX > maxRayDistance) break;
            sideDistX   += deltaDistX;
            currentMapX += stepX;
            hitSide = WallType::VERTICAL;
        } else {
            if (sideDistY > maxRayDistance) break;
            sideDistY   += deltaDistY;
            currentMapY += stepY;
            hitSide = WallType::HORIZONTAL;
        }

        // Stop if the ray has left the map bounds or entered a non-zero (wall) tile.
        if (currentMapY < 0 || currentMapY >= mapHeight ||
            currentMapX < 0 || currentMapX >= mapWidth) {
            hitWall = true;
        } else if (worldMap[currentMapY][currentMapX] != 0) {    // Skills Audit Group B: Multi-dimensional arrays (2-D grid cell lookup by row and column)
            hitWall = true;
        }
    }

    // Compute the perpendicular (non-fisheye) distance using the DDA step formula.
    float wallDistance;
    if (hitSide == WallType::VERTICAL) {
        wallDistance = (currentMapX - startX + (1.0f - stepX) / 2.0f) / rayDirectionX;    // Skills Audit Group A: Complex scientific/mathematical model (perpendicular wall distance formula)
    } else {
        wallDistance = (currentMapY - startY + (1.0f - stepY) / 2.0f) / rayDirectionY;
    }

    // Compute the wallU texture coordinate from the exact hit position along the wall face.
    float wallU;
    if (hitSide == WallType::VERTICAL) {
        float exactY = startY + wallDistance * (rayDirectionY / rayDirectionX);    // Skills Audit Group B: Simple scientific/mathematical model (exact hit Y via direction ratio)
        wallU = exactY - floor(exactY);    // Skills Audit Group B: Simple scientific/mathematical model (fractional part gives texture U in [0,1])
        // Flip U on east-facing walls so the texture reads left-to-right.
        if (rayDirectionX > 0) wallU = 1.0f - wallU;    // Skills Audit Group B: Simple user defined algorithms (U-flip for correct texture orientation)
    } else {
        float exactX = startX + wallDistance * (rayDirectionX / rayDirectionY);
        wallU = exactX - floor(exactX);
        if (rayDirectionY < 0) wallU = 1.0f - wallU;
    }

    // Fisheye correction: project the hit distance onto the camera plane using a dot product.    // Skills Audit Group A: Complex scientific/mathematical model (dot-product fisheye correction)
    float dirX = cos(playerAngle);    // Skills Audit Group B: Simple scientific/mathematical model (forward direction vector from player angle)
    float dirY = sin(playerAngle);
    wallDistance = wallDistance * (rayDirectionX * dirX + rayDirectionY * dirY);    // Skills Audit Group B: Simple scientific/mathematical model (dot product projection)

    float hitPointX = startX + rayDirectionX * wallDistance;    // Skills Audit Group B: Simple scientific/mathematical model (reconstructing world hit position)
    float hitPointY = startY + rayDirectionY * wallDistance;

    // Package all computed values into a RayHit for use by the renderer.
    RayHit rayHit;    // Skills Audit Group B: Records (RayHit struct aggregates all per-ray results)
    rayHit.distance        = wallDistance;
    rayHit.hitX            = hitPointX;
    rayHit.hitY            = hitPointY;
    rayHit.wallU           = wallU;
    rayHit.angle           = atan2(rayDirectionY, rayDirectionX);    // Skills Audit Group B: Simple scientific/mathematical model (ray angle from direction components)
    rayHit.hitVerticalWall = (hitSide == WallType::VERTICAL);

    // Read the wall type from the map cell that was hit; default to 1 if out of bounds.
    if (currentMapY >= 0 && currentMapY < mapHeight &&
        currentMapX >= 0 && currentMapX < mapWidth) {
        rayHit.wallType = worldMap[currentMapY][currentMapX];    // Skills Audit Group B: Multi-dimensional arrays (reading wall type from 2-D grid)
    } else {
        rayHit.wallType = 1;
    }

    return rayHit;
}

std::vector<RayHit> Raycaster::castAllRays(Player& player, int screenWidth) {
    std::vector<RayHit> rayResults;

    // Build the camera plane from the player's angle and FOV.
    float playerAngle = player.getAngle();
    float dirX = cos(playerAngle);    // Skills Audit Group B: Simple scientific/mathematical model (forward direction from angle)
    float dirY = sin(playerAngle);

    float FOVRadians  = player.getFieldOfView() * (M_PI / 180.0f);    // Skills Audit Group B: Simple scientific/mathematical model (degrees to radians)
    float planeLength = tan(FOVRadians / 2.0f);    // Skills Audit Group B: Simple scientific/mathematical model (half-FOV tangent gives camera plane length)
    // The camera plane is perpendicular to the forward direction, scaled by the FOV half-tangent.
    float planeX      = -dirY * planeLength;    // Skills Audit Group A: Complex scientific/mathematical model (camera plane perpendicular vector)
    float planeY      =  dirX * planeLength;

    for (int x = 0; x < screenWidth; x++) {
        // cameraX maps screen column x to [-1, 1] across the camera plane.
        float cameraX = 2.0f * x / static_cast<float>(screenWidth) - 1.0f;    // Skills Audit Group B: Simple scientific/mathematical model (screen column to normalised camera coordinate)

        float rayDirX = dirX + planeX * cameraX;    // Skills Audit Group B: Simple scientific/mathematical model (ray direction as forward + lateral offset)
        float rayDirY = dirY + planeY * cameraX;

        rayResults.push_back(castSingleRay(player.getX(), player.getY(), rayDirX, rayDirY, playerAngle));
    }
    return rayResults;
}

void Raycaster::setMaxDistance(float distance) { maxRayDistance = distance; }
