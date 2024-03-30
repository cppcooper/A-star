#include "global.h"
#include "thebuilder.h"

dir_generator RDGenerator;

void **Matrix2D(int ncols, int nrows, int element_size, int first_row = 0, int first_col = 0) {
    //nrows and ncols are the total number of rows and columns,first_row and first_col are the starting rows and columns(default is 0,it can be negative too !!)
    void **p;
    int alignment;
    long i;
    if (ncols < 1 || nrows < 1) { return (NULL); }
    i = ncols * sizeof(void *);
    //  align the addr of the data to be a multiple of sizeof(long double),this helps easy fetching and fast processing in some processors
    alignment = i % sizeof(long double);
    if (alignment != 0) {
        alignment = sizeof(long double) - alignment;
    }
    i += ncols * nrows * element_size + alignment;
    p = (void **) malloc((size_t) i);
    if (p != NULL) {
        //compute the address of matrix[first_row][0]
        p[0] = (char *) (p + ncols) + alignment - first_row * element_size;
        for (i = 1; i < ncols; i++)
            // compute the address of matrix[first_row+i][0]
            p[i] = (char *) (p[i - 1]) + nrows * element_size;
        //compute the address of matrix[0][0]
        p -= first_col;
    } else
        //cout<<"Memory allocation failure"<<endl;
        exit(-20);
    return (p);
}

char **thebuilder::generate(maze_specs &gen_size) {
    REGENERATEMAZE:
    RDGenerator.seed_me();
    Cartographer.Reset();
    int &x = gen_size.first;
    int &y = gen_size.second;
    x = x % 2 ? x : x + 1;
    y = y % 2 ? y : y + 1;
    size.first = x;
    size.second = y;
    if (maze != nullptr) {
        delete maze;

    }
    maze = (char **) Matrix2D(x, y, sizeof(char));


    //Initialization - First Stage (Values)
    coords loop_pos;
    short &ix = loop_pos.first;
    short &iy = loop_pos.second;
    for (ix = 0; ix < x; ++ix) {
        for (iy = 0; iy < y; ++iy) {
            //Cells of the maze need initial values

            maze[ix][iy] = 5;
            Cartographer.Register_Cell(loop_pos);
        }
    }
    char state = 0;
    //Initialization - Second Stage (Visited Cells)

    for (ix = 1, iy = 0; iy != -1;) {
        Cartographer << loop_pos;
        switch (state) {
            case 0:
                ++ix;
                if ((ix + 1) == x) {
                    ++state;

                }
                break;
            case 1:
                ++iy;
                if ((iy + 1) == y) {
                    ++state;

                }
                break;
            case 2:
                --ix;
                if (ix == 0) {
                    ++state;

                }
                break;
            case 3:
                --iy;
                break;
        }
    }

    //Initialization - Third Stage (Entrance)
    seed_value2 = time(NULL);
    //srand( 1417031426 );
    //seed_value2 = 1417629337;
    srand(seed_value2);
    int tx, ty;


    char star_script = -1;
    switch (rand() % 4) //The maze needs a start and finish
    {
        case 0: // Top
            tx = rand() % (x - 1);
            tx = tx % 2 ? tx : (tx <= (x / 2) ? tx + 1 : tx - 1);
            maze[tx][y - 1] = star_script;
            position.first = tx;
            position.second = y - 1;
            Cartographer.Register_Cell(position, true);
            current_path.emplace(position);
            break;
        case 1: // Right
            ty = rand() % (y - 1);
            ty = ty % 2 ? ty : (ty <= (y / 2) ? ty + 1 : ty - 1);
            maze[x - 1][ty] = star_script;
            position.first = x - 1;
            position.second = ty;
            Cartographer.Register_Cell(position, true);
            current_path.emplace(position);
            break;
        case 2: // Bottom
            tx = rand() % (x - 1);
            tx = tx % 2 ? tx : (tx <= (x / 2) ? tx + 1 : tx - 1);
            maze[tx][0] = star_script;
            position.first = tx;
            position.second = 0;
            Cartographer.Register_Cell(position, true);
            current_path.emplace(position);
            break;
        case 3: // Left
            ty = rand() % (y - 1);
            ty = ty % 2 ? ty : (ty <= (y / 2) ? ty + 1 : ty - 1);
            maze[0][ty] = star_script;
            position.first = 0;
            position.second = ty;
            Cartographer.Register_Cell(position, true);
            current_path.emplace(position);
            break;
        default:
            //Something bad happened

            break;
    }

    // Post-Initialization
    // Start putting up walls, building paths
    int Area = (x * y);// -(2 * x) - (2 * y) + 2;


    while (Cartographer.Count() != Area) {
        if (current_path.size() == 0 && Cartographer.Count() > 4) {
            goto REGENERATEMAZE;
        }
        //
        //draw(maze, size, position);

        build_path();
        paths.push_back(current_path);
        if (backtrack()) {
            position = current_path.top();

        } else {

        }
        //draw( maze, size, std::make_pair( 0, 0 ) );
        seed_value;
    }
    return maze;
}

inline direction thebuilder::get_directions() {
    short &x = position.first;
    short &y = position.second;

    direction R = 0;
    for (int i = 0; i < 4; ++i) {
        short ix, iy;
        int j = 1 << (2 * i);
        switch (j) {
            case UP:
                iy = y + 1;
                if (((x % 2) == 1) && ((iy) < size.second)) {
                    if (!Cartographer.Check_Cell(std::make_pair(x, iy))) {
                        R |= UP;
                    }
                }
                break;
            case RIGHT:
                ix = x + 1;
                if (((y % 2) == 1) && ((ix) < size.first)) {
                    if (!Cartographer.Check_Cell(std::make_pair(ix, y))) {
                        R |= RIGHT;
                    }
                }
                break;
            case DOWN:
                iy = y - 1;
                if (((x % 2) == 1) && ((iy) >= 0)) {
                    if (!Cartographer.Check_Cell(std::make_pair(x, iy))) {
                        R |= DOWN;
                    }
                }
                break;
            case LEFT:
                ix = x - 1;
                if (((y % 2) == 1) && ((ix) >= 0)) {
                    if (!Cartographer.Check_Cell(std::make_pair(ix, y))) {
                        R |= LEFT;
                    }
                }
                break;
            default:
                break;
        }
    }
    return R;
}

inline void thebuilder::build_path() {
    while (RDGenerator << get_directions()) {
        //draw( maze, size, position );

        switch (RDGenerator.rnd()) {
            case UP:
                build_up();
                break;
            case RIGHT:
                build_right();
                break;
            case DOWN:
                build_down();
                break;
            case LEFT:
                build_left();
                break;
            default:
                break;
        }
        //
        //draw(maze, size, position);

        current_path.emplace(position);
    }
}

inline void thebuilder::build_up() {
    short &x = position.first;
    short &y = position.second;
    //
    //Cartographer << position;
    maze[x][++y] = rand() % 5;
    tour();
}

inline void thebuilder::build_down() {
    short &x = position.first;
    short &y = position.second;
    //
    //Cartographer << position;
    maze[x][--y] = rand() % 5;
    tour();
}

inline void thebuilder::build_left() {
    short &x = position.first;
    short &y = position.second;
    //
    //Cartographer << position;
    maze[--x][y] = rand() % 5;
    tour();
}

inline void thebuilder::build_right() {
    short &x = position.first;
    short &y = position.second;
    //
    //Cartographer << position;
    maze[++x][y] = rand() % 5;
    tour();
}

inline void thebuilder::tour() {
    const coords last = current_path.top();// write code to deal with this
    coords tourist = last;


    const short Lx = last.first;
    const short Ly = last.second;
    const short Cx = position.first;
    const short Cy = position.second;
    short &Tx = tourist.first;
    short &Ty = tourist.second;

    short left, right, up, down;
    left = last.first - 1;
    left = left >= 0 ? left : 0;
    right = last.first + 1;
    right = right < size.first ? right : size.first - 1;
    up = last.second + 1;
    up = up < size.second ? up : size.second - 1;
    down = last.second - 1;
    down = down >= 0 ? down : 0;
    Cartographer << tourist;
    if (Cx == Lx) {
        //Evidence of vertical movements
        if (Ly < Cy) {
            //North movement verified

            Tx = left;
            Cartographer << tourist;
            Ty = down;
            Cartographer << tourist;
            Tx = Lx;
            Cartographer << tourist;
            Tx = right;
            Cartographer << tourist;
            Ty = Ly;
            Cartographer << tourist;
        } else {
            //South movement assumed

            Tx = left;
            Cartographer << tourist;
            Ty = up;
            Cartographer << tourist;
            Tx = Lx;
            Cartographer << tourist;
            Tx = right;
            Cartographer << tourist;
            Ty = Ly;
            Cartographer << tourist;
        }
    } else if (Cy == Ly) {
        //Evidence of horizontal movements
        if (Lx < Cx) {
            //East movement verified

            Ty = up;
            Cartographer << tourist;
            Tx = left;
            Cartographer << tourist;
            Ty = Ly;
            Cartographer << tourist;
            Ty = down;
            Cartographer << tourist;
            Tx = Lx;
            Cartographer << tourist;
        } else {
            //West movement assumed

            Ty = up;
            Cartographer << tourist;
            Tx = right;
            Cartographer << tourist;
            Ty = Ly;
            Cartographer << tourist;
            Ty = down;
            Cartographer << tourist;
            Tx = Lx;
            Cartographer << tourist;
        }
    }
}

inline bool thebuilder::backtrack() {
    if (current_path.size() > 0) {
        Cartographer << current_path.top();
    }
    while (current_path.size() > 0) {
        current_path.pop();
        if (current_path.size() == 0) {
            return false;
        }
        coords temp = current_path.top();

        short &x = temp.first;
        short &y = temp.second;
        //draw(maze, size, temp);
        //system("pause");
        int i = rand() % 4;
        for (int count = 0; count < 4; ++count) {
            int ix = 0, iy = 0;
            int j = 1 << (2 * i++);
            i = i <= 4 ? i : 0;
            switch (j) {
                case UP:
                    iy = y + 2;
                    if (((x % 2) == 1) && ((iy) < size.second)) {
                        if (!Cartographer.Check_Cell(std::make_pair(x, iy))) {
                            maze[x][--iy] = 0;
                            current_path.emplace(std::make_pair(x, iy));
                            return true;
                        }
                    }
                    break;
                case RIGHT:
                    ix = x + 2;
                    if (((y % 2) == 1) && ((ix) < size.first)) {
                        if (!Cartographer.Check_Cell(std::make_pair(ix, y))) {
                            maze[--ix][y] = 0;
                            current_path.emplace(std::make_pair(ix, y));
                            return true;
                        }
                    }
                    break;
                case DOWN:
                    iy = y - 2;
                    if (((x % 2) == 1) && ((iy) >= 0)) {
                        if (!Cartographer.Check_Cell(std::make_pair(x, iy))) {
                            maze[x][++iy] = 0;
                            current_path.emplace(std::make_pair(x, iy));
                            return true;
                        }
                    }
                    break;
                case LEFT:
                    ix = x - 2;
                    if (((y % 2) == 1) && ((ix) >= 0)) {
                        if (!Cartographer.Check_Cell(std::make_pair(ix, y))) {
                            maze[++ix][y] = 0;
                            current_path.emplace(std::make_pair(ix, y));
                            return true;
                        }
                    }
                    break;
                default:
                    break;
            }
        }
    }
    return false;
}