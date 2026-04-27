#pragma once
#include <vector>
#include <memory>
#include "RandomNumberGenerator.h"

class Rectangle;
class Division;
class HorizontalDivision;
class VerticalDivision;

class Point {
public:
    int y, x;
    Point(int yCoord, int xCoord);
};

enum class Orientation {
    HORIZONTAL,
    VERTICAL
};

enum CellType {
    WALL    = 1,
    PASSAGE = 0
};

class Division {
protected:
    int line;
    int passagePosition;

public:
    Division(int divisionLine, int passage);
    int getLine() const;

    virtual std::vector<Point> getWallPoints() const = 0;
    virtual Point getPassagePoint() const = 0;
    virtual ~Division() = default;
};

class HorizontalDivision : public Division {
private:
    int startCoord;
    int wallLength;

public:
    HorizontalDivision(int divisionLine, int passageX, int chamberX, int chamberWidth);
    std::vector<Point> getWallPoints() const override;
    Point getPassagePoint() const override;
};

class VerticalDivision : public Division {
private:
    int startCoord;
    int wallLength;

public:
    VerticalDivision(int divisionLine, int passageY, int chamberY, int chamberHeight);
    std::vector<Point> getWallPoints() const override;
    Point getPassagePoint() const override;
};

class Rectangle {
public:
    int x, y, width, height;
    Rectangle(int startX, int startY, int rectWidth, int rectHeight);
    bool canSubdivide() const;
    std::vector<Rectangle> split(const Division& division) const;
};

class RecursiveDivisionMazeGenerator {
private:
    int width, height;
    std::vector<std::vector<int>> maze;
    RandomNumberGenerator rng;

    void initializeGrid();
    void subdivide(Rectangle chamber);
    std::unique_ptr<Division> createDivision(const Rectangle& chamber);
    Orientation chooseDivisionOrientation(const Rectangle& chamber);
    void drawWall(const Division& division);
    void createPassage(const Division& division);

public:
    RecursiveDivisionMazeGenerator(int w, int h, int seed);

    void generateMaze();
    void printMaze() const;
    void printMazeAsArray() const;

    std::vector<int> getMazeAsVector() const;
    int getWidth() const;
    int getHeight() const;
    const std::vector<std::vector<int>>& getMaze() const;
};
