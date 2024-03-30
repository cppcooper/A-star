#include "debug_funcs.h"
#include <sstream>

inline std::string coords_ToString(std::pair<int, int> position) {
    std::stringstream temp;
    temp << " --X:" << position.first << ", Y:" << position.second << "-- ";
    return temp.str();
}
