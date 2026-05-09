#pragma once
#include <random>
#include <vector>
#include <algorithm>

// Wraps a Mersenne Twister engine to provide seeded, reproducible random numbers.
class RandomNumberGenerator {
private:
    // Mersenne Twister — high-quality PRNG used for all random operations.    // Skills Audit Group A: Complex scientific/mathematical/robotics/control/business model
    std::mt19937 generator;

public:
    // Seeds the generator from the current system time for a different sequence each run.
    RandomNumberGenerator();
    // Seeds the generator with a specific value for reproducible sequences.
    RandomNumberGenerator(unsigned int seed);

    // Returns a uniformly distributed integer in [min, max].
    int  randomInt(int min, int max);
    // Returns a random odd integer in [min, max]; adjusts bounds inward if needed.
    int  randomOdd(int min, int max);
    // Returns a random even integer in [min, max]; adjusts bounds inward if needed.
    int  randomEven(int min, int max);
    bool randomBoolean();
    // Reseeds the generator without constructing a new object.
    void setSeed(unsigned int seed);

    // Fisher-Yates in-place shuffle of any vector type.
    template<typename T>
    void shuffle(std::vector<T>& vec) {
        for (int i = vec.size() - 1; i > 0; i--) {
            std::uniform_int_distribution<int> dist(0, i);
            std::swap(vec[i], vec[dist(generator)]);
        }
    }
};
