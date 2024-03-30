#include "cell_manager.h"

void cell_mgr::operator<<(coords position) {
    auto itr = Cells.find(position);
    if (!itr->second) {
        itr->second = true;
        Cells_Visited++;
    }
}

void cell_mgr::Register_Cell(coords position, bool visited) {
    auto TempVar = Cells.emplace(position, visited);
    if (!TempVar.second) {
        // No addition was made to the Map
        if (!TempVar.first->second && visited) {
            Cells_Visited++;
        }
        TempVar.first->second = visited;
    } else {
        // Added position to Map
        if (visited) {
            Cells_Visited++;
        }
    }
}

bool cell_mgr::Check_Cell(coords reference_point) {
    if (Cells[reference_point]) {
        return true;
    }
    return false;
}

int cell_mgr::Count() {
    return Cells_Visited;
}

void cell_mgr::Reset() {
    Cells_Visited = 0;
    Cells.clear();
    while (!Q_Branch.empty()) {
        Q_Branch.pop();
    }
}
