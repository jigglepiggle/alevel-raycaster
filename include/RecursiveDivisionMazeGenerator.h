#pragma once
#include <vector>
#include <memory>
#include "RandomNumberGenerator.h"

class Rectangle;
class Division;
class HorizontalDivision;
class VerticalDivision;

// Represents a single grid coordinate used when specifying wall and passage positions.
class Point {
public:
    int y, x;
    Point(int yCoord, int xCoord);
};

// Whether the next dividing wall runs horizontally or vertically across the chamber.
enum class Orientation {
    HORIZONTAL,
    VERTICAL
};

// Integer values stored in the maze grid to distinguish walls from passages.
enum CellType {
    WALL    = 1,
    PASSAGE = 0
};

// Abstract base for a single wall drawn across a chamber during recursive subdivision.    // Skills Audit Group A: Complex user-defined use of OOP model (polymorphism, interfaces)
class Division {
protected:
    int line;             // Row or column index where the wall is drawn.
    int passagePosition;  // Index of the single gap left in the wall.

public:
    Division(int divisionLine, int passage);
    int getLine() const;

    // Returns every grid cell that should be set to WALL.
    virtual std::vector<Point> getWallPoints() const = 0;
    // Returns the single cell that remains PASSAGE in the wall.
    virtual Point getPassagePoint() const = 0;
    virtual ~Division() = default;
};

// A horizontal wall spanning the full width of the chamber at a given row.    // Skills Audit Group A: Complex user-defined use of OOP model (inheritance)
class HorizontalDivision : public Division {
private:
    int startCoord;   // Left edge column of the wall.
    int wallLength;   // Number of cells the wall spans.

public:
    HorizontalDivision(int divisionLine, int passageX, int chamberX, int chamberWidth);
    std::vector<Point> getWallPoints() const override;
    Point getPassagePoint() const override;
};

// A vertical wall spanning the full height of the chamber at a given column.    // Skills Audit Group A: Complex user-defined use of OOP model (inheritance)
class VerticalDivision : public Division {
private:
    int startCoord;   // Top edge row of the wall.
    int wallLength;   // Number of cells the wall spans.

public:
    VerticalDivision(int divisionLine, int passageY, int chamberY, int chamberHeight);
    std::vector<Point> getWallPoints() const override;
    Point getPassagePoint() const override;
};

// Axis-aligned rectangular region of the maze grid that can be recursively subdivided.
class Rectangle {
public:
    int x, y, width, height;
    Rectangle(int startX, int startY, int rectWidth, int rectHeight);
    // Returns false if the chamber is too small to place a wall with a passage.
    bool canSubdivide() const;
    // Splits the chamber at the division line and returns the two sub-chambers.    // Skills Audit Group A: Recursive algorithms
    std::vector<Rectangle> split(const Division& division) const;
};

// Generates a perfect maze using the recursive division algorithm.    // Skills Audit Group A: Recursive algorithms
class RecursiveDivisionMazeGenerator {
private:
    int width, height;
    // 2-D grid storing WALL and PASSAGE values.    // Skills Audit Group A: Advanced matrix operations
    std::vector<std::vector<int>> maze;
    RandomNumberGenerator rng;

    // Fills the grid: border cells become WALL, interior cells become PASSAGE.
    void initializeGrid();
    // Recursively subdivides the chamber by drawing a wall and recursing into each half.
    void subdivide(Rectangle chamber);
    // Picks orientation, chooses the wall line and passage position, and returns a Division.
    std::unique_ptr<Division> createDivision(const Rectangle& chamber);
    // Chooses HORIZONTAL or VERTICAL based on chamber aspect ratio.
    Orientation chooseDivisionOrientation(const Rectangle& chamber);
    // Sets every wall-point of the division to WALL in the grid.
    void drawWall(const Division& division);
    // Sets the passage point of the division back to PASSAGE.
    void createPassage(const Division& division);

public:
    RecursiveDivisionMazeGenerator(int w, int h, int seed);

    // Initialises the grid and runs the full recursive subdivision.
    void generateMaze();
    void printMaze() const;
    void printMazeAsArray() const;

    // Returns the maze flattened to a 1-D vector in row-major order.
    std::vector<int> getMazeAsVector() const;
    int getWidth() const;
    int getHeight() const;
    const std::vector<std::vector<int>>& getMaze() const;
};
