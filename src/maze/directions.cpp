#include "directions.h"

unsigned int seed_value = 0;
unsigned int seed_value2 = 0;

dir_generator::dir_generator(double complexity) {
    bitmask = 0;
    last = 0;
    repeat = std::bernoulli_distribution(1.01 - complexity);
    distribution = std::uniform_int_distribution<int>(0, 7);
    seed_me();
}

void dir_generator::seed_me() {
    srand(time(NULL));
    seed_value = rand();
    //generator.seed( 29763 );
    //seed_value = 16212;
    generator.seed(seed_value);
}

direction dir_generator::rnd() {
    if ((last & bitmask) && (repeat(generator))) {
        return last;
    } else {
        direction R;
        do {
            R = 1 << distribution(generator);
            // Repeats if R is a 0 in the bitmask
        } while (R & ~bitmask);
        last = R;
        return R;
    }
}

bool dir_generator::operator<<(direction possibilities) {
    bitmask = possibilities;
    if (possibilities != 0)
        return true;
    return false;
}

void dir_generator::operator>>(direction &result) {
    result = this->rnd();
}

void dir_generator::set_complexity(double complexity) {
    repeat = std::bernoulli_distribution(1.01 - complexity);
}