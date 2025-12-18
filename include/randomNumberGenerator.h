#pragma once

#include <random>
#include <vector>
#include <algorithm>

/**
 * @brief A wrapper class for random number generation using Mersenne Twister
 * 
 * This class provides a convenient interface for generating various types of random numbers
 * including integers, booleans, odd/even numbers, and shuffling collections.
 * It uses std::mt19937 internally for high-quality pseudorandom number generation.
 */
class RandomNumberGenerator {
private:
    std::mt19937 generator;  ///< Mersenne Twister random number generator
    
public:
    /**
     * @brief Default constructor with automatic seeding using current time
     * 
     * Seeds the random number generator with the current system time,
     * providing different sequences on each program run.
     */
    RandomNumberGenerator();
    
    /**
     * @brief Constructor with custom seed for reproducible sequences
     * 
     * @param seed The seed value for the random number generator
     *             Using the same seed will produce identical sequences
     */
    RandomNumberGenerator(unsigned int seed);
    
    /**
     * @brief Generate a random integer within a specified range (inclusive)
     * 
     * @param min The minimum value (inclusive)
     * @param max The maximum value (inclusive)
     * @return A random integer between min and max
     * @throws std::invalid_argument if min > max
     */
    int randomInt(int min, int max);
    
    /**
     * @brief Generate a random odd number within a specified range (inclusive)
     * 
     * Automatically adjusts the range bounds to ensure they are odd.
     * Useful for maze generation where odd coordinates are often required.
     * 
     * @param min The minimum value (will be adjusted to odd if even)
     * @param max The maximum value (will be adjusted to odd if even)
     * @return A random odd integer within the adjusted range
     */
    int randomOdd(int min, int max);
    
    /**
     * @brief Generate a random even number within a specified range (inclusive)
     * 
     * Automatically adjusts the range bounds to ensure they are even.
     * Useful for maze generation where even coordinates are often required.
     * 
     * @param min The minimum value (will be adjusted to even if odd)
     * @param max The maximum value (will be adjusted to even if odd)
     * @return A random even integer within the adjusted range
     */
    int randomEven(int min, int max);
    
    /**
     * @brief Generate a random boolean value
     * 
     * @return true or false with equal probability (50/50)
     */
    bool randomBoolean();
    
    /**
     * @brief Shuffle a vector using the Fisher-Yates algorithm
     * 
     * This template function can shuffle any vector type. It uses the Fisher-Yates
     * shuffle algorithm to ensure uniform distribution of all possible permutations.
     * The shuffle is performed in-place, modifying the original vector.
     * 
     * @tparam T The type of elements in the vector
     * @param vec Reference to the vector to be shuffled (modified in-place)
     */
    template<typename T>
    void shuffle(std::vector<T>& vec);
    
    /**
     * @brief Reset the random number generator with a new seed
     * 
     * @param seed The new seed value for the generator
     */
    void setSeed(unsigned int seed);
};

/**
 * @brief Template implementation of the Fisher-Yates shuffle algorithm
 * 
 * This implementation must be in the header file due to C++ template requirements.
 * The Fisher-Yates shuffle ensures that each possible permutation of the input
 * vector has equal probability of being generated.
 * 
 * Algorithm:
 * 1. Start from the last element and work backwards
 * 2. For each element at position i, generate a random index j between 0 and i
 * 3. Swap elements at positions i and j
 * 4. Continue until reaching the first element
 * 
 * Time Complexity: O(n) where n is the size of the vector
 * Space Complexity: O(1) - in-place shuffling
 */
template<typename T>
void RandomNumberGenerator::shuffle(std::vector<T>& vec) {
    // Fisher-Yates shuffle algorithm
    for (int i = vec.size() - 1; i > 0; i--) {
        // Generate random index between 0 and i (inclusive)
        std::uniform_int_distribution<int> dist(0, i);
        int j = dist(generator);
        
        // Swap elements at positions i and j
        std::swap(vec[i], vec[j]);
    }
}