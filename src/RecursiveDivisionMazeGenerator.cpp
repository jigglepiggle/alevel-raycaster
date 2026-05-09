#include "RecursiveDivisionMazeGenerator.h"
#include <iostream>
#include <ctime>

// Store the row and column coordinates for this grid point.
Point::Point(int yCoord, int xCoord) : y(yCoord), x(xCoord) {}    // Skills Audit Group B: Records (Point struct stores a grid coordinate pair)

// Store the row/column index of the wall and the position of its single gap.
Division::Division(int divisionLine, int passage) : line(divisionLine), passagePosition(passage) {}    // Skills Audit Group B: Records (base Division struct stores wall line and passage position)

int Division::getLine() const {
    return line;
}

// Store the left edge and width of the horizontal wall segment.
HorizontalDivision::HorizontalDivision(int divisionLine, int passageX, int chamberX, int chamberWidth) 
    : Division(divisionLine, passageX), startCoord(chamberX), wallLength(chamberWidth) {}    // Skills Audit Group A: Complex user-defined use of OOP model (inheritance — delegating to Division base)

// Returns every cell in the wall row from startCoord to startCoord + wallLength.
std::vector<Point> HorizontalDivision::getWallPoints() const {
    std::vector<Point> points;
    for (int x = startCoord; x < startCoord + wallLength; x++) {    // Skills Audit Group B: Simple user defined algorithms (sequential wall point generation)
        points.push_back(Point(line, x));    // Skills Audit Group B: Records (Point struct created per wall cell)
    }
    return points;
}

// The passage is the one cell in the wall row that stays open.
Point HorizontalDivision::getPassagePoint() const {
    return Point(line, passagePosition);    // Skills Audit Group B: Records (Point struct identifying the passage cell)
}

// Store the top edge and height of the vertical wall segment.
VerticalDivision::VerticalDivision(int divisionLine, int passageY, int chamberY, int chamberHeight) 
    : Division(divisionLine, passageY), startCoord(chamberY), wallLength(chamberHeight) {}    // Skills Audit Group A: Complex user-defined use of OOP model (inheritance — delegating to Division base)

// Returns every cell in the wall column from startCoord to startCoord + wallLength.
std::vector<Point> VerticalDivision::getWallPoints() const {
    std::vector<Point> points;
    for (int y = startCoord; y < startCoord + wallLength; y++) {    // Skills Audit Group B: Simple user defined algorithms (sequential wall point generation)
        points.push_back(Point(y, line));    // Skills Audit Group B: Records (Point struct created per wall cell)
    }
    return points;
}

Point VerticalDivision::getPassagePoint() const {
    return Point(passagePosition, line);    // Skills Audit Group B: Records (Point struct identifying the passage cell)
}

// Store the top-left corner and dimensions of this rectangular chamber.
Rectangle::Rectangle(int startX, int startY, int rectWidth, int rectHeight) 
    : x(startX), y(startY), width(rectWidth), height(rectHeight) {}    // Skills Audit Group B: Records (Rectangle struct stores chamber bounds)

// A chamber needs at least 3 tiles in both dimensions to fit a wall with a passage.
bool Rectangle::canSubdivide() const {
    return width >= 3 && height >= 3;    // Skills Audit Group B: Simple user defined algorithms (minimum size check for subdivision)
}

// Splits the chamber at the division line, returning the two resulting sub-chambers.    // Skills Audit Group A: Recursive algorithms
std::vector<Rectangle> Rectangle::split(const Division& division) const {
    std::vector<Rectangle> subChambers;

    if (dynamic_cast<const HorizontalDivision*>(&division)) {    // Skills Audit Group A: Complex user-defined use of OOP model (polymorphism — runtime type dispatch)
        // Top half runs from y to the wall row; bottom half starts one row below the wall.
        subChambers.push_back(Rectangle(x, y, width, division.getLine() - y));    // Skills Audit Group B: Simple scientific/mathematical model (sub-chamber height from division line offset)
        subChambers.push_back(Rectangle(x, division.getLine() + 1, width, (y + height) - (division.getLine() + 1)));
    } else {
        // Left half runs from x to the wall column; right half starts one column to the right.
        subChambers.push_back(Rectangle(x, y, division.getLine() - x, height));
        subChambers.push_back(Rectangle(division.getLine() + 1, y, (x + width) - (division.getLine() + 1), height));
    }
    return subChambers;
}

// Force odd dimensions so wall and passage positions always land on valid coordinates.
RecursiveDivisionMazeGenerator::RecursiveDivisionMazeGenerator(int w, int h, int seed) 
    : width(w), height(h), rng(seed == 0 ? RandomNumberGenerator() : RandomNumberGenerator(seed)) {    // Skills Audit Group B: Generation of objects based on simple OOP model
    if (width % 2 == 0) width++;     // Skills Audit Group B: Simple user defined algorithms (force odd width)
    if (height % 2 == 0) height++;   // Skills Audit Group B: Simple user defined algorithms (force odd height)
}

// Fills the 2-D grid: outer border is WALL, all interior cells start as PASSAGE.    // Skills Audit Group B: Multi-dimensional arrays (2-D grid initialisation)
void RecursiveDivisionMazeGenerator::initializeGrid() {
    maze.assign(height, std::vector<int>(width, CellType::WALL));    // Skills Audit Group B: Multi-dimensional arrays (2-D grid allocation filled with WALL)

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {    // Skills Audit Group A: Advanced matrix operations (nested loop over 2-D grid)
            if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                maze[y][x] = CellType::WALL;     // Skills Audit Group B: Multi-dimensional arrays (writing to 2-D grid cell)
            } else {
                maze[y][x] = CellType::PASSAGE;
            }
        }
    }
}

// Recursively subdivides the chamber by placing a wall, carving a passage, and recurring.    // Skills Audit Group A: Recursive algorithms
void RecursiveDivisionMazeGenerator::subdivide(Rectangle chamber) {
    // Base case: chamber is too small to subdivide further.
    if (!chamber.canSubdivide()) {
        return;
    }

    // Choose orientation, wall position, and passage position for this chamber.
    std::unique_ptr<Division> division = createDivision(chamber);    // Skills Audit Group A: Complex user-defined use of OOP model (polymorphism — factory returns concrete Division subtype)
    
    drawWall(*division);
    createPassage(*division);

    // Recurse into both halves produced by the division.
    std::vector<Rectangle> subChambers = chamber.split(*division);
    for (auto& subChamber : subChambers) {
        subdivide(subChamber);    // Skills Audit Group A: Recursive algorithms (subdivide called on each sub-chamber)
    }
}

// Picks a random wall row/column and passage position within the chamber bounds.
std::unique_ptr<Division> RecursiveDivisionMazeGenerator::createDivision(const Rectangle& chamber) {
    Orientation orientation = chooseDivisionOrientation(chamber);

    if (orientation == Orientation::HORIZONTAL) {
        // Wall row must be an odd index so it falls on an interior grid line.
        int divisionLineY = rng.randomOdd(chamber.y + 1, chamber.y + chamber.height - 2);    // Skills Audit Group B: Simple user defined algorithms (odd-index wall placement)
        // Passage must be an even index to align with open passage cells.
        int passagePointX = rng.randomEven(chamber.x, chamber.x + chamber.width - 1);    // Skills Audit Group B: Simple user defined algorithms (even-index passage placement)
        return std::make_unique<HorizontalDivision>(divisionLineY, passagePointX, chamber.x, chamber.width);
    } else {
        int divisionLineX = rng.randomOdd(chamber.x + 1, chamber.x + chamber.width - 2);
        int passagePointY = rng.randomEven(chamber.y, chamber.y + chamber.height - 1);
        return std::make_unique<VerticalDivision>(divisionLineX, passagePointY, chamber.y, chamber.height);
    }
}

// Taller chambers are split horizontally; wider chambers are split vertically; square chambers are random.
Orientation RecursiveDivisionMazeGenerator::chooseDivisionOrientation(const Rectangle& chamber) {
    if (chamber.width > chamber.height) {
        return Orientation::VERTICAL;    // Skills Audit Group B: Simple user defined algorithms (aspect-ratio-based orientation choice)
    } else if (chamber.height > chamber.width) {
        return Orientation::HORIZONTAL;
    } else {
        return rng.randomBoolean() ? Orientation::HORIZONTAL : Orientation::VERTICAL;    // Skills Audit Group B: Simple user defined algorithms (random coin flip for square chambers)
    }
}

// Sets every point returned by the division to WALL in the grid, with bounds checking.
void RecursiveDivisionMazeGenerator::drawWall(const Division& division) {
    for (const auto& point : division.getWallPoints()) {    // Skills Audit Group A: Complex user-defined use of OOP model (polymorphism — virtual getWallPoints dispatch)
        if (point.x >= 0 && point.x < height && point.y >= 0 && point.y < width) {
            maze[point.y][point.x] = CellType::WALL;    // Skills Audit Group B: Multi-dimensional arrays (writing WALL value to 2-D grid cell)
        }
    }
}

// Carves the single passage cell back to PASSAGE so the two halves remain connected.
void RecursiveDivisionMazeGenerator::createPassage(const Division& division) {
    Point point = division.getPassagePoint();    // Skills Audit Group A: Complex user-defined use of OOP model (polymorphism — virtual getPassagePoint dispatch)
    if (point.x >= 0 && point.x < height && point.y >= 0 && point.y < width) {
        maze[point.y][point.x] = CellType::PASSAGE;    // Skills Audit Group B: Multi-dimensional arrays (writing PASSAGE value to 2-D grid cell)
    }
}

void RecursiveDivisionMazeGenerator::generateMaze() {
    // Initialise the open grid, then subdivide the entire interior rectangle.
    initializeGrid();
    subdivide(Rectangle(1, 1, width - 2, height - 2));    // Skills Audit Group A: Recursive algorithms (entry point for full-grid recursive subdivision)

    // Mark the entrance on the left border (row 1) and exit on the right border (second-to-last row).
    maze[1][0]              = 2;    // Skills Audit Group B: Multi-dimensional arrays (writing entrance marker to 2-D grid)
    maze[height - 2][width - 1] = 2;
}

// Prints the maze to stdout using '#' for walls and ' ' for passages.
void RecursiveDivisionMazeGenerator::printMaze() const {
    for (const auto& row : maze) {    // Skills Audit Group B: Multi-dimensional arrays (iterating rows of 2-D grid)
        for (int cell : row)
            std::cout << (cell == CellType::WALL ? '#' : ' ');
        std::cout << '\n';
    }
}

// Prints the maze as a C-style integer array literal for debugging or embedding.
void RecursiveDivisionMazeGenerator::printMazeAsArray() const {
    std::cout << "int map[] = {\n";
    for (int y = 0; y < height; y++) {
        std::cout << "    ";  
        for (int x = 0; x < width; x++) {    // Skills Audit Group B: Multi-dimensional arrays (row-major traversal of 2-D grid)
            std::cout << maze[y][x];
            if (y < height - 1 || x < width - 1)
                std::cout << ",";
        }
        std::cout << "\n";
    }
    std::cout << "};\n";
}

// Flattens the 2-D grid to a 1-D vector in row-major order.
std::vector<int> RecursiveDivisionMazeGenerator::getMazeAsVector() const {
    std::vector<int> flatMaze;
    flatMaze.reserve(width * height);    // Skills Audit Group B: Simple user defined algorithms (pre-allocation by calculated capacity)
    
    for (const auto& row : maze) {
        for (int cell : row) {
            flatMaze.push_back(cell);    // Skills Audit Group A: Advanced matrix operations (flattening 2-D grid to 1-D vector)
        }
    }
    return flatMaze;
}

int RecursiveDivisionMazeGenerator::getWidth() const { return width; }

int RecursiveDivisionMazeGenerator::getHeight() const { return height; }

const std::vector<std::vector<int>>& RecursiveDivisionMazeGenerator::getMaze() const { 
    return maze;    // Skills Audit Group B: Multi-dimensional arrays (returning reference to the 2-D grid)
}
