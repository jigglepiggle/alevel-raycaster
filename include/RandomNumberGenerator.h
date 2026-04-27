#pragma once
#include <random>
#include <vector>
#include <algorithm>

class RandomNumberGenerator {
private:
    std::mt19937 generator;

public:
    RandomNumberGenerator();
    RandomNumberGenerator(unsigned int seed);

    int  randomInt(int min, int max);
    int  randomOdd(int min, int max);
    int  randomEven(int min, int max);
    bool randomBoolean();
    void setSeed(unsigned int seed);

    template<typename T>
    void shuffle(std::vector<T>& vec) {
        for (int i = vec.size() - 1; i > 0; i--) {
            std::uniform_int_distribution<int> dist(0, i);
            std::swap(vec[i], vec[dist(generator)]);
        }
    }
};
