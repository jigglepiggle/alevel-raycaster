#pragma once
#include "WorldMap.h"

// Represents the player's position, orientation and movement within the maze.    // Skills Audit Group A: Generation of objects based on simple OOP model
class Player {
private:
  float x;              // World-space X position in tiles.
  float y;              // World-space Y position in tiles.
  float angle;          // Facing direction in radians.
  float FOV;            // Field of view in degrees.
  float moveSpeed;      // Tiles moved per frame.
  float rotateSpeed;    // Rotation speed multiplier.
  // Pointer to the map used for collision detection; not owned by Player.
  const WorldMap* worldMap;

public:
  explicit Player(float startX, float startY, 
                  float startAngle, float fov, float rotateSpeed, float moveSpeed);

    // Stores the world map pointer so movement methods can check for wall collisions.
    void setWorldMap(const WorldMap* map);

    // Advances the player in the direction they are facing, if the destination is not a wall.
    void moveForward();
    // Moves the player in the opposite direction to their facing angle.
    void moveBackward();
    // Decreases the facing angle to rotate left.
    void turnLeft();
    // Increases the facing angle to rotate right.
    void turnRight();
    float getX() const;
    float getY() const;
    float getAngle() const;
    float getFieldOfView() const;
    // Teleports the player to (nx, ny) if the destination cell is not a wall.
    void setPosition(float nx, float ny);
    void setAngle(float na);
};
