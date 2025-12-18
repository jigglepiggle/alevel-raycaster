#pragma once

#include "recursiveDivisionMazeGenerator.h"
#include <iostream>
#include <ctime>

// Point class implementation
Point::Point(int yCoord, int xCoord) : y(yCoord), x(xCoord) {}

// Division class implementation
Division::Division(int divisionLine, int passage) : line(divisionLine), passagePosition(passage) {}

int Division::getLine() const {
    return line;
}

// HorizontalDivision class implementation
HorizontalDivision::HorizontalDivision(int divisionLine, int passageX, int chamberX, int chamberWidth) 
    : Division(divisionLine, passageX), startCoord(chamberX), wallLength(chamberWidth) {}

std::vector<Point> HorizontalDivision::getWallPoints() const {
    std::vector<Point> points;
    for (int x = startCoord; x < startCoord + wallLength; x++) {
        points.push_back(Point(line, x));
    }
    return points;
}

Point HorizontalDivision::getPassagePoint() const {
    return Point(line, passagePosition);
}

// VerticalDivision class implementation
VerticalDivision::VerticalDivision(int divisionLine, int passageY, int chamberY, int chamberHeight) 
    : Division(divisionLine, passageY), startCoord(chamberY), wallLength(chamberHeight) {}

std::vector<Point> VerticalDivision::getWallPoints() const {
    std::vector<Point> points;
    for (int y = startCoord; y < startCoord + wallLength; y++) {
        points.push_back(Point(y, line));
    }
    return points;
}

Point VerticalDivision::getPassagePoint() const {
    return Point(passagePosition, line);
}

// Rectangle class implementation
Rectangle::Rectangle(int startX, int startY, int rectWidth, int rectHeight) 
    : x(startX), y(startY), width(rectWidth), height(rectHeight) {}

bool Rectangle::canSubdivide() const {
    return width >= 3 && height >= 3;
}

std::vector<Rectangle> Rectangle::split(const Division& division) const {
    std::vector<Rectangle> subChambers;

    if (dynamic_cast<const HorizontalDivision*>(&division)) {
        subChambers.push_back(Rectangle(x, y, width, division.getLine() - y));
        subChambers.push_back(Rectangle(x, division.getLine() + 1, width, (y + height) - (division.getLine() + 1)));
    } else {
        subChambers.push_back(Rectangle(x, y, division.getLine() - x, height));
        subChambers.push_back(Rectangle(division.getLine() + 1, y, (x + width) - (division.getLine() + 1), height));
    }
    return subChambers;
}

// RecursiveDivisionMazeGenerator class implementation
RecursiveDivisionMazeGenerator::RecursiveDivisionMazeGenerator(int w, int h, int seed) 
    : width(w), height(h), rng(seed == 0 ? RandomNumberGenerator() : RandomNumberGenerator(seed)) {
    if (width % 2 == 0) width++;
    if (height % 2 == 0) height++;
}

void RecursiveDivisionMazeGenerator::initializeGrid() {
    maze.assign(height, std::vector<int>(width, CellType::WALL));

    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (x == 0 || x == width - 1 || y == 0 || y == height - 1) {
                maze[y][x] = CellType::WALL;
            } else {
                maze[y][x] = CellType::PASSAGE;
            }
        }
    }
}

void RecursiveDivisionMazeGenerator::subdivide(Rectangle chamber) {
    if (!chamber.canSubdivide()) {
        return;
    }

    std::unique_ptr<Division> division = createDivision(chamber);
    
    drawWall(*division);
    createPassage(*division);

    std::vector<Rectangle> subChambers = chamber.split(*division);
    for (auto& subChamber : subChambers) {
        subdivide(subChamber);
    }
}

std::unique_ptr<Division> RecursiveDivisionMazeGenerator::createDivision(const Rectangle& chamber) {
    Orientation orientation = chooseDivisionOrientation(chamber);

    if (orientation == Orientation::HORIZONTAL) {
        int divisionLineY = rng.randomOdd(chamber.y + 1, chamber.y + chamber.height - 2);
        int passagePointX = rng.randomEven(chamber.x, chamber.x + chamber.width - 1);
        return std::make_unique<HorizontalDivision>(divisionLineY, passagePointX, chamber.x, chamber.width);
    } else {
        int divisionLineX = rng.randomOdd(chamber.x + 1, chamber.x + chamber.width - 2);
        int passagePointY = rng.randomEven(chamber.y, chamber.y + chamber.height - 1);
        return std::make_unique<VerticalDivision>(divisionLineX, passagePointY, chamber.y, chamber.height);
    }
}

Orientation RecursiveDivisionMazeGenerator::chooseDivisionOrientation(const Rectangle& chamber) {
    if (chamber.width > chamber.height) {
        return Orientation::VERTICAL;
    } else if (chamber.height > chamber.width) {
        return Orientation::HORIZONTAL;
    } else {
        return rng.randomBoolean() ? Orientation::HORIZONTAL : Orientation::VERTICAL;
    }
}

void RecursiveDivisionMazeGenerator::drawWall(const Division& division) {
    for (const auto& point : division.getWallPoints()) {
        if (point.x >= 0 && point.x < height && point.y >= 0 && point.y < width) {
            maze[point.y][point.x] = CellType::WALL;
        }
    }
}

void RecursiveDivisionMazeGenerator::createPassage(const Division& division) {
    Point point = division.getPassagePoint();
    if (point.x >= 0 && point.x < height && point.y >= 0 && point.y < width) {
        maze[point.y][point.x] = CellType::PASSAGE;
    }
}

void RecursiveDivisionMazeGenerator::generateMaze() {
    initializeGrid();
    subdivide(Rectangle(1, 1, width - 2, height - 2));

    maze[1][0] = CellType::PASSAGE;
    maze[height - 2][width - 1] = CellType::PASSAGE;
}

void RecursiveDivisionMazeGenerator::printMaze() const {
    for (const auto& row : maze) {
        for (int cell : row) {
            std::cout << (cell == CellType::WALL ? '#' : ' ');
        }
        std::cout << '\n';
    }
}

void RecursiveDivisionMazeGenerator::printMazeAsArray() const {
    std::cout << "int map[] = {\n";
    for (int y = 0; y < height; y++) {
        std::cout << "    ";  
        for (int x = 0; x < width; x++) {
            std::cout << maze[y][x];
            if (y < height - 1 || x < width - 1) {
                std::cout << ",";
            }
            //std::cout << " ";
        }
        std::cout << "\n";
    }
    std::cout << "};\n";
}

std::vector<int> RecursiveDivisionMazeGenerator::getMazeAsVector() const {
    std::vector<int> flatMaze;
    flatMaze.reserve(width * height);
    
    for (const auto& row : maze) {
        for (int cell : row) {
            flatMaze.push_back(cell);
        }
    }
    return flatMaze;
}

int RecursiveDivisionMazeGenerator::getWidth() const { 
    return width; 
}

int RecursiveDivisionMazeGenerator::getHeight() const { 
    return height; 
}

const std::vector<std::vector<int>>& RecursiveDivisionMazeGenerator::getMaze() const { 
    return maze; 
}