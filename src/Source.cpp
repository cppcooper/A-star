#include "global.h"
#include "keyboard.h"
#include "debug_funcs.h"
#include "thebuilder.h"
#include "THEpathfinder.h"
#include <sstream>

#define _SHOW_RESULTS
#define _DRAW_MAZE
#define _PAUSE

void Get_Dimensions(maze_specs &size) {
    static bool once_before = false;
    if (once_before) {
        bool valid_selection = false;
        while (!valid_selection) {
            std::cout << "\nDo you want to use previous size? Y/N\n";
            switch (key_wait()) {
                case 'N':
                case 'n':
                    once_before = false;
                    valid_selection = true;
                    Get_Dimensions(size);
                    break;
                case 'Y':
                case 'y':
                    return void(0);
                    break;
                default:
                    //No valid selection, repeats current loop
                    std::cout << "Please use valid input!" << std::endl;
                    valid_selection = false;
                    break;
            }
        }
    } else {
        bool valid_selection = false;
        std::cout << "Please enter the dimensions of the map";
        while (!valid_selection) {
            try {
                std::cout << std::endl << "X: ";
                std::cin >> size.first;
                if (std::cin.fail()) {
                    throw std::cin.rdstate();
                }
                std::cout << std::endl << "Y: ";
                std::cin >> size.second;
                if (std::cin.fail()) {
                    throw std::cin.rdstate();
                }
                valid_selection = true;
            }
            catch (...) {
                std::cout << "Received invalid input" << std::endl;
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                valid_selection = false;
            }
        }
        once_before = true;
    }
}

int main() {
    thebuilder bob;
    maze_specs size;

    std::vector<double> times_list;
    std::vector<unsigned int> seed_list;
    char **maze;

    unsigned int iterations = 0;
    std::cout << "How many maps do you want to generate and test?" << std::endl;
    bool valid_selection = false;
    while (!valid_selection) {
        try {
            std::cin >> iterations;
            if (std::cin.fail()) {
                throw std::cin.rdstate();
            }
            valid_selection = true;
        }
        catch (...) {
            std::cout << "Received invalid input" << std::endl;
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            valid_selection = false;
        }
    }
    for (int i = 1; i <= iterations; ++i) {
        Get_Dimensions(size);
        maze = bob.generate(size);
#ifdef _DRAW_MAZE
        draw(maze, size, std::make_pair(0, 0));
#endif
        //system( "pause" );

        pathfinder astar;
        astar.setmap(maze, size);
        coords start;
        coords goal;
        start.first = start.second = 1;
        maze[start.first][start.second] = -3;
        goal.first = size.first - 2;
        goal.second = size.second - 2;
        maze[goal.first][goal.second] = -2;
        std::chrono::time_point<std::chrono::high_resolution_clock> time_start, time_end;
        time_start = std::chrono::high_resolution_clock::now();
        Path *path_found = astar.search(start, goal);
        time_end = std::chrono::high_resolution_clock::now();
#ifdef _SHOW_RESULTS
        path_found->ShowPath();
#endif
        //delete EndNode_onPath;

        delete path_found;
        //maze = nullptr;
        std::chrono::duration<double> elapsed_seconds = (time_end - time_start);
        times_list.push_back(elapsed_seconds.count());
        seed_list.push_back(seed_value);
        seed_list.push_back(seed_value2);
        std::cout << std::endl << i << ".  " << elapsed_seconds.count() << std::endl;
#ifdef _PAUSE
        system("pause");
#endif
    }
    delete maze;
    double total = 0;
    int s = 0;
    for (int i = 0; i < times_list.size(); ++i) {
        std::cout << i + 1 << ".  Time: " << times_list[i] << "\t Seed 1: " << seed_list[s++] << "\t Seed 2: "
                  << seed_list[s++] << std::endl;
        total += times_list[i];
    }
    std::cout << "\nAverage search time for your tests" << std::endl;
    total = total / times_list.size();
    std::cout << total << " seconds" << std::endl;
    system("pause");
}

inline void draw(char **maze, maze_specs size, coords cursor) {
    //system( "cls" );
    std::stringstream buffer;
    for (int y = size.second - 1; y >= 0; --y) {
        for (int x = 0; x < size.first; ++x) {
            if ((x == cursor.first) && (y == cursor.second)) {
                buffer << "^";
            } else {
                switch (maze[x][y]) {
                    case -3:
                        buffer << "S";
                        break;
                    case -2:
                        buffer << "G";
                        break;
                    case -1:
                        buffer << "#";
                        break;
                    case 0:
                        buffer << "0";
                        break;
                    case 1:
                        buffer << "1";
                        break;
                    case 2:
                        buffer << "2";
                        break;
                    case 3:
                        buffer << "3";
                        break;
                    case 4:
                        buffer << "4";
                        break;
                    case 5:
                        buffer << "5";
                        break;
                    default:
                        buffer << " ";
                        break;
                }
            }
        }
        buffer << std::endl;
    }
    std::cout << buffer.str();
}