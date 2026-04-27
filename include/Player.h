#pragma once
#include "WorldMap.h"

class Player {
private:
  float x;
  float y;
  float angle;
  float FOV;
  float moveSpeed;
  float rotateSpeed;
  const WorldMap* worldMap;

public:
  explicit Player(float startX, float startY, 
                  float startAngle, float fov, float rotateSpeed, float moveSpeed);

    void setWorldMap(const WorldMap* map);

    void moveForward();
    void moveBackward();
    void turnLeft();
    void turnRight();
    float getX() const;
    float getY() const;
    float getAngle() const;
    float getFieldOfView() const;
    void setPosition(float nx, float ny);
    void setAngle(float na);
};
