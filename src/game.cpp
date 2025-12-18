#pragma once

#include <vector>
#include <math.h>


class WorldMap {
private:
    std::vector<std::vector<int>> mapData;
    int height;
    int width;
    
public:
    WorldMap(std::vector<std::vector<int>> mapData, int height, int width) : mapData(mapData), height(height), width(width) {}

    // These two functions currently do the same thing, getWallType is only required for if multiple different tiles are added
    bool isWall(int x, int y) { return mapData[y][x]; };
    int getWallType(int x, int y) { return mapData[y][x]; };

    std::vector<std::vector<int>> getMap() { return mapData; };

    int getWidth() { return width; };
    int getHeight() { return height; };
};

class Player {
private:
    float x;
    float y;
    float angle;
    float FOV;
    float moveSpeed;
    float rotateSpeed;

public:
    Player(float startX, float startY, float startAngle, float fov, float rotateSpeed = 2) : x(startX), y(startY), angle(startAngle), FOV(fov), rotateSpeed(rotateSpeed) {} //map variables

    void update() {};

    void moveForward() { x += cos(angle) * 0.2; y += sin(angle) * 0.2; };
    void moveBackwards() { x -= cos(angle) * 0.2; y -= sin(angle) * 0.2; };
    void turnLeft() { angle -= 0.1 * rotateSpeed; };
    void turnRight() { angle += 0.1 * rotateSpeed; };
    
    float getX() const { return x; };
    float getY() const { return y; };
    float getAngle() const { return angle; };
    float getFieldOfView() const { return FOV; };

    void setPosition(float nx, float ny) { x = nx; y = ny; };
    void setAngle(float nAngle) { angle = nAngle; };
};
