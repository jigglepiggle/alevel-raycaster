#pragma once

struct RayHit {
  float distance;
  float hitX;
  float hitY;
  float wallU;
  int wallType;
  bool hitVerticalWall;
  float angle;
};
