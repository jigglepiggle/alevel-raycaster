#include <iostream>
#include <chrono>
#include "depthFirstMazeGenerator.h"
#include "recursiveDivisionMazeGenerator.h"

// Simple function to print a separator line
void printSeparator(const std::string& text = "") {
    if (text.empty()) {
        std::cout << std::string(50, '-') << "\n";
    } else {
        std::cout << "=== " << text << " ===\n";
    }
}

// Function to print basic stats using existing getter methods
void printBasicStats(DepthFirstMazeGenerator& dfGen, RecursiveDivisionMazeGenerator& rdGen) {
    printSeparator("MAZE STATISTICS");
    
    std::cout << "Depth-First Maze:\n";
    std::cout << "  Dimensions: " << dfGen.getWidth() << "x" << dfGen.getHeight() << "\n";
    
    std::cout << "\nRecursive Division Maze:\n";
    std::cout << "  Dimensions: " << rdGen.getWidth() << "x" << rdGen.getHeight() << "\n";
    
    std::cout << "\nAlgorithm Characteristics:\n";
    std::cout << "• Depth-First: Creates long winding paths with tree structure\n";
    std::cout << "• Recursive Division: Creates geometric chambers with multiple paths\n";
}

// Main comparison function using existing methods
void compareMazes(int width, int height, unsigned int seed) {
    // Ensure odd dimensions
    if (width % 2 == 0) width++;
    if (height % 2 == 0) height++;
    
    printSeparator("MAZE COMPARISON");
    std::cout << "Dimensions: " << width << "x" << height << " | Seed: " << seed << "\n\n";
    
    // Generate depth-first maze
    DepthFirstMazeGenerator dfGen(width, height, seed);
    dfGen.generateMaze();
    
    // Generate recursive division maze
    RecursiveDivisionMazeGenerator rdGen(width, height, seed);
    rdGen.generateMaze();
    
    // Print first maze
    std::cout << "DEPTH-FIRST SEARCH MAZE:\n";
    dfGen.printMaze();
    
    std::cout << "\n";
    
    // Print second maze
    std::cout << "RECURSIVE DIVISION MAZE:\n";
    rdGen.printMaze();
    
    std::cout << "\n";
    
    // Print stats using existing methods
    printBasicStats(dfGen, rdGen);
}

// Function to export mazes using existing methods
void exportMazes(int width, int height, unsigned int seed) {
    if (width % 2 == 0) width++;
    if (height % 2 == 0) height++;
    
    printSeparator("MAZE ARRAYS");
    
    DepthFirstMazeGenerator dfGen(width, height, seed);
    dfGen.generateMaze();
    
    RecursiveDivisionMazeGenerator rdGen(width, height, seed);
    rdGen.generateMaze();
    
    std::cout << "Depth-First Maze Array:\n";
    dfGen.printMazeAsArray();
    
    std::cout << "\nRecursive Division Maze Array:\n";
    rdGen.printMazeAsArray();
}

// Function to compare multiple maze pairs
void compareMultipleMazes(int width, int height, unsigned int baseSeed, int count) {
    for (int i = 0; i < count; i++) {
        std::cout << "\n";
        compareMazes(width, height, baseSeed + i * 1000);
    }
}

// Interactive function
void runInteractiveComparison() {
    int width, height;
    unsigned int seed;
    
    printSeparator("INTERACTIVE COMPARISON");
    std::cout << "Enter width: ";
    std::cin >> width;
    std::cout << "Enter height: ";
    std::cin >> height;
    std::cout << "Enter seed (0 for random): ";
    std::cin >> seed;
    
    if (seed == 0) {
        seed = static_cast<unsigned int>(std::time(nullptr));
    }
    
    compareMazes(width, height, seed);
}

// Main function
int main() {
    // Basic comparison
    //compareMazes(49, 49, 12345);
    
    exportMazes(63, 63, 0);

    return 0;
}