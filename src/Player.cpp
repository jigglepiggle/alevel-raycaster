#include "Player.h"
#include <cmath>

// Initialise all member variables from the constructor parameters.
Player::Player(float startX, float startY, float startAngle,
               float fov, float rotateSpeed, float moveSpeed)
    : x(startX), y(startY), angle(startAngle), FOV(fov),    // Skills Audit Group B: Records (member fields store the player's complete state)
      rotateSpeed(rotateSpeed), moveSpeed(moveSpeed), worldMap(nullptr) {}

// Store the world map pointer so collision checks can query isWall().
void Player::setWorldMap(const WorldMap* map) {
    worldMap = map;
}

void Player::moveForward() {
    // Compute the candidate position by projecting along the facing direction.    // Skills Audit Group A: Complex scientific/mathematical model (trigonometric movement projection)
    float newX = x + cos(angle) * moveSpeed;    // Skills Audit Group B: Simple scientific/mathematical model (cosine x-component of movement vector)
    float newY = y + sin(angle) * moveSpeed;    // Skills Audit Group B: Simple scientific/mathematical model (sine y-component of movement vector)
    // Only move if the destination tile is not a wall.
    if (worldMap && !worldMap->isWall(static_cast<int>(newX), static_cast<int>(newY))) {
        x = newX;
        y = newY;
    }
}

// Identical to moveForward but subtracts the direction vector instead of adding it.
void Player::moveBackward() {
    float newX = x - cos(angle) * moveSpeed;    // Skills Audit Group B: Simple scientific/mathematical model (reversed movement vector)
    float newY = y - sin(angle) * moveSpeed;
    if (worldMap && !worldMap->isWall(static_cast<int>(newX), static_cast<int>(newY))) {
        x = newX;
        y = newY;
    }
}

// Decrement the angle to rotate counter-clockwise.
void Player::turnLeft()  { angle -= 0.1f * rotateSpeed; }    // Skills Audit Group B: Simple scientific/mathematical model (scaled angle decrement)
// Increment the angle to rotate clockwise.
void Player::turnRight() { angle += 0.1f * rotateSpeed; }    // Skills Audit Group B: Simple scientific/mathematical model (scaled angle increment)

float Player::getX()           const { return x; }
float Player::getY()           const { return y; }
float Player::getAngle()       const { return angle; }
float Player::getFieldOfView() const { return FOV; }

// Sets the player's position directly, ignoring walls — used by the debug teleport.
void Player::setPosition(float nx, float ny) {
    if (worldMap && worldMap->isWall(static_cast<int>(nx), static_cast<int>(ny))) return;
    x = nx;    // Skills Audit Group B: Records (updating player position fields directly)
    y = ny;
}

void Player::setAngle(float newAngle) {
    angle = newAngle;
}
