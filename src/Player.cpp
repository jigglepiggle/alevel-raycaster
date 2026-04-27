#include "Player.h"
#include <cmath>

Player::Player(float startX, float startY, float startAngle,
               float fov, float rotateSpeed, float moveSpeed)
    : x(startX), y(startY), angle(startAngle), FOV(fov),
      rotateSpeed(rotateSpeed), moveSpeed(moveSpeed), worldMap(nullptr) {}

void Player::setWorldMap(const WorldMap* map) {
    worldMap = map;
}

void Player::moveForward() {
    float newX = x + cos(angle) * moveSpeed;
    float newY = y + sin(angle) * moveSpeed;
    if (worldMap && !worldMap->isWall(static_cast<int>(newX), static_cast<int>(newY))) {
        x = newX;
        y = newY;
    }
}

void Player::moveBackward() {
    float newX = x - cos(angle) * moveSpeed;
    float newY = y - sin(angle) * moveSpeed;
    if (worldMap && !worldMap->isWall(static_cast<int>(newX), static_cast<int>(newY))) {
        x = newX;
        y = newY;
    }
}

void Player::turnLeft()  { angle -= 0.1f * rotateSpeed; }
void Player::turnRight() { angle += 0.1f * rotateSpeed; }

float Player::getX()           const { return x; }
float Player::getY()           const { return y; }
float Player::getAngle()       const { return angle; }
float Player::getFieldOfView() const { return FOV; }

void Player::setPosition(float nx, float ny) {
    if (worldMap && worldMap->isWall(static_cast<int>(nx), static_cast<int>(ny))) return;
    x = nx;
    y = ny;
}

void Player::setAngle(float newAngle) {
    angle = newAngle;
}
