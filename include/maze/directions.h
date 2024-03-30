#ifndef _DIRECTIONS_H_
#define _DIRECTIONS_H_

#include "debug_funcs.h"
#include <random>
#include <ctime>
#include <cinttypes>

typedef int8_t direction;
enum {
    UP = 1, UP_RIGHT = 2, RIGHT = 4, DOWN_RIGHT = 8, DOWN = 16, DOWN_LEFT = 32, LEFT = 64, UP_LEFT = 128
};

class dir_generator {
private:
    direction bitmask;//Allowed output
    direction last;//Previously generated output
    std::bernoulli_distribution repeat;
    std::uniform_int_distribution<int> distribution;
    std::default_random_engine generator;
protected:
public:
    dir_generator(double complexity = 0.75);

    bool operator<<(direction possibilities);

    void operator>>(direction &result);

    void set_complexity(double complexity);

    void seed_me();

    direction rnd();
};

#endif