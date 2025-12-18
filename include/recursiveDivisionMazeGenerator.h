#pragma once

#include <vector>
#include <memory>
#include "randomNumberGenerator.h"

// Forward declarations
class Rectangle;
class Division;
class HorizontalDivision;
class VerticalDivision;

// Point class definition
class Point {
public:
    int y, x;
    Point(int yCoord, int xCoord);
};

// Orientation enum definition
enum class Orientation {
    HORIZONTAL,
    VERTICAL
};

// CellType enum definition
enum CellType {
    WALL = 1,
    PASSAGE = 0
};

// Division base class definition
class Division {
protected:
    int line; // The coordinate of the division line (x for vertical, y for horizontal)
    int passagePosition; // The coordinate of the passage along the division line

public:
    Division(int divisionLine, int passage);
    int getLine() const;
    
    // Pure virtual functions to be implemented by derived classes
    virtual std::vector<Point> getWallPoints() const = 0;
    virtual Point getPassagePoint() const = 0;
    virtual ~Division() = default;
};

// HorizontalDivision class definition
class HorizontalDivision : public Division {
private:
    int startCoord; // x-coordinate where the wall starts
    int wallLength; // width of the wall segment

public:
    HorizontalDivision(int divisionLine, int passageX, int chamberX, int chamberWidth);
    std::vector<Point> getWallPoints() const override;
    Point getPassagePoint() const override;
};

// VerticalDivision class definition
class VerticalDivision : public Division {
private:
    int startCoord; // y-coordinate where the wall starts
    int wallLength; // height of the wall segment

public:
    VerticalDivision(int divisionLine, int passageY, int chamberY, int chamberHeight);
    std::vector<Point> getWallPoints() const override;
    Point getPassagePoint() const override;
};

// Rectangle class definition
class Rectangle {
public:
    int x, y, width, height; // x, y are top-left coordinates

    Rectangle(int startX, int startY, int rectWidth, int rectHeight);
    bool canSubdivide() const;
    std::vector<Rectangle> split(const Division& division) const;
};

// MazeGenerator class definition
class RecursiveDivisionMazeGenerator {
private: 
    // Maze dimensions - should be odd numbers for proper wall/path structure
    int width, height;
    
    // 2D maze representation: 1 = wall, 0 = path
    std::vector<std::vector<int>> maze;

    RandomNumberGenerator rng;

    void initializeGrid();
    void subdivide(Rectangle chamber);
    std::unique_ptr<Division> createDivision(const Rectangle& chamber);
    Orientation chooseDivisionOrientation(const Rectangle& chamber);
    void drawWall(const Division& division);
    void createPassage(const Division& division);

public:
    /**
     * Constructor for MazeGenerator
     * @param w Width of the maze (should be odd for proper structure)
     * @param h Height of the maze (should be odd for proper structure)
     * @param seed Random seed for reproducible mazes (0 = use current time)
     */
    RecursiveDivisionMazeGenerator(int w, int h, int seed);
    
    void generateMaze();
    void printMaze() const;
    void printMazeAsArray() const;
    std::vector<int> getMazeAsVector() const;
    
    int getWidth() const;
    int getHeight() const;
    const std::vector<std::vector<int>>& getMaze() const;
};