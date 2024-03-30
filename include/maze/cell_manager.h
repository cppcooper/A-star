#ifndef _CELL_MANAGER_H_
#define _CELL_MANAGER_H_
#define LOG_CUSTOM_LEVEL

#include "debug_funcs.h"

#include <list>
#include <map>
#include <queue>

typedef std::pair<short, short> coords;

class cell_mgr {
private:
    int Cells_Visited = 0;
    std::map<coords, bool> Cells;
    std::queue<std::map<coords, bool>::iterator> Q_Branch;
protected:
public:
    void operator<<(coords position); //Binary search for insertion spot
    void Register_Cell(coords position, bool visited = false);

    bool Check_Cell(coords reference_point);

    int Count();

    void Reset();
};

#endif