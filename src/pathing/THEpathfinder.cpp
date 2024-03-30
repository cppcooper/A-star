#include "THEpathfinder.h"
#include <algorithm>

extern void draw(char **maze, maze_specs, coords cursor);

pathfinder::pathfinder() {
#ifndef _DEQUE_OPEN_LIST
    open_list.reserve(300);
#endif
}

void pathfinder::sort() {
    if (!open_list.empty()) {
#ifdef _DEQUE_OPEN_LIST
        std::deque<node*>::iterator highest_priority = open_list.end() - 1;
        std::deque<node*>::iterator itor = open_list.begin();
#else
        std::vector<node *>::iterator highest_priority = open_list.end() - 1;
        std::vector<node *>::iterator itor = open_list.begin();
#endif
        for (itor; itor != open_list.end(); ++itor) {
            if ((*itor)->f < (*highest_priority)->f)
                highest_priority = itor;
        }
        if (highest_priority != open_list.end() - 1) {
            std::iter_swap(highest_priority, open_list.end() - 1);
        }
    } else {

    }
}

inline unsigned int pathfinder::hash(coords pos) {
    union local_union {
        short myarray[2];
        unsigned int R;
    };
    local_union my_var;
    my_var.myarray[0] = pos.first;
    my_var.myarray[1] = pos.second;
    return my_var.R;
}

unsigned int pathfinder::get_goal_distance(coords pos) {
    short &gx = goal.first;
    short &gy = goal.second;
    short &x = pos.first;
    short &y = pos.second;

    unsigned int dx = gx > x ? gx - x : x - gx;
    unsigned int dy = gy > y ? gy - y : y - gy;
    return dx + dy;
}

bool pathfinder::canMove(coords pos) {
    if ((pos.first >= size.first) || (pos.second >= size.second) || (pos.first < 0) || (pos.second < 0)) {
        return false;
    }
    int value = themap[pos.first][pos.second];
    if (value == -2 || value == -3) {
        return true;
    } else if (value < 0) {
        return false;
    }
    return true;
}

void pathfinder::cleanUp() {
    open_list.push_back(current);
    for (auto N : open_list) {
#ifdef _MAP_CLOSED_LIST
        closed_list[this->hash(N->pos)] = N;
#else
        auto iter = closed_list.begin();
        for ( ; iter != closed_list.end( ); ++iter )
        {
            auto T = *iter;
            if ( T->x == N->x && T->y == N->y )
            {
                break;
            }
        }
        if ( iter == closed_list.end() )
        {
            closed_list.push_back( N );
        }
#endif
    }
    open_list.clear();
    for (auto iter = closed_list.begin(); iter != closed_list.end(); ++iter) {
#ifdef _MAP_CLOSED_LIST
        node *T = iter->second;
#else
        node* T = *iter;
#endif
        delete T;
    }
    closed_list.clear();
    //map = nullptr;
}

void pathfinder::setmap(char **map, maze_specs size) {
    this->size = size;
    if (this->themap != nullptr && this->themap != map) {
        delete this->themap;
    }
    this->themap = map;
}

Path *pathfinder::search(coords start, const coords goal) {
    //If we're not done cleaning up our last search
    if (C1.joinable()) {
        C1.join();
    }

    //No map has been given to search through
    if (themap == nullptr || !canMove(start) || !canMove(goal)) {
        return nullptr;
    }

    //These are our locals to keep track of our "current" data points

    //Setting the member versions of these variables
    this->start = start;
    this->goal = goal;


    //Need to create our start node
    current = new node(start);
    current->is_on_openlist = true;
    current->h = get_goal_distance(current->pos);

    //Now to track the node
    open_list.push_back(current);
#ifdef _MAP_CLOSED_LIST
    closed_list.emplace(hash(current->pos), current);
#else
    closed_list.push_back( current );
#endif

    //We got nodes to pop
    while (!open_list.empty()) {


        //Let's grab the cheapest node and then take it off our container

#ifndef _DEQUE_OPEN_LIST
#ifdef _FRONT_SORT_
        //Using a vector (front side sorted)
        current = *open_list.begin();
        open_list.erase( open_list.begin() );
#else
        //Using a vector (back side sorted)
        current = *(open_list.end() - 1);
        open_list.erase((open_list.end() - 1));
#endif
#else
        //Using a deque
        current = open_list.front();
        open_list.pop_front();
#endif
        current->is_on_openlist = false;
        //draw( themap, size, current->pos );

        //We have found our destination?
        if (current->pos == goal) {

            //cleanUp();

            //We did find our way, now we need to provide that path
            Path *R = new Path();
            R->TracePath(current);

            //Spawn a thread to run the cleanup
            //C1 = std::thread(&pathfinder::cleanUp, this);
            cleanUp();

            //All done here, time to return the path for use
            return R;
        }

        //draw( themap, size, current->pos );
        expand_node(current);


        //Sorts one item to the "Top"
#ifndef _INSERT_
        sort();
#endif
    }

    //This should never happen

    return nullptr;
}

void pathfinder::expand_node(node *current) {
    short dirx[] = {1, -1, 0, 0};
    short diry[] = {0, 0, 1, -1};

    //for loop for up, down, right, and left expansions
    for (int i = 0; i < 4; ++i) {


        //Let's put together our expansion coordinates
        coords Expansion = std::make_pair(current->x + dirx[i], current->y + diry[i]);


        //If our coordinates can be expanded to we actually expand
        if (canMove(Expansion)) {
            evaluate_expansion(Expansion);
        }
    }
}

void pathfinder::evaluate_expansion(coords Expansion) {
    float f, g, h;
    f = g = h = 0;
    //We need to know the terrain cost
    unsigned int value =
            themap[Expansion.first][Expansion.second] < 0 ? 0 : themap[Expansion.first][Expansion.second] + 1;
    g = current->g + value;
#ifdef _MAP_CLOSED_LIST
    //We might as well cache our coordinate's hash value
    unsigned int ihash = this->hash(Expansion);

    //Look for our coordinates
    auto temp_itor = closed_list.find(ihash);
#else
    auto temp_itor = closed_list.begin();
    for ( ; temp_itor != closed_list.end(); ++temp_itor )
    {
        auto N = *temp_itor;
        if ( N->x == Expansion.first && N->y == Expansion.second )
        {
            break;
        }
    }
#endif
    node *temp;

    //Did we find the coordinates?
    if (temp_itor != closed_list.end()) {
#ifdef _MAP_CLOSED_LIST
        temp = temp_itor->second;
#else
        temp = *temp_itor;
#endif

        //Was the last time we were here cheaper?
        if (temp->g <= g) {
            //Last time was cheaper so disregard this loop iteration and move on to the next

        } else {
            //This time is cheaper than the previous time, so we must update our records


            //Checking to see if this node is already on the open list, if so we remove it
            if (temp->is_on_openlist) {
#ifdef _INSERT_
                binary_remove(temp, temp->f);
#else
                remove( temp );
#endif
            } else {
                temp->is_on_openlist = true;
            }
            temp->parent = current;
            temp->f = f = temp->h + g;
            temp->g = g;

            //Putting it in
#ifdef _INSERT_
            sorted_insert(temp, f);
#else
            open_list.push_back( temp );
#endif
        }
    } else {
        //We haven't seen this position before

        h = heuristic * get_goal_distance(Expansion);
        f = g + h;


        //This position needs a node associated with it
        temp = new node(Expansion, current, f, g, h);
        temp->is_on_openlist = true;

        //Putting it in
#ifdef _MAP_CLOSED_LIST
        closed_list.emplace(ihash, temp);
#else
        closed_list.push_back( temp );
#endif
#ifdef _INSERT_
        sorted_insert(temp, f);
#else
        open_list.push_back( temp );
#endif
    }
}

void pathfinder::sorted_insert(node *Cur, float search_value) {
    int infinite = 0;
    //Debugging check for Sorted List
    //We want a small list
    /*if ( open_list.size() > 10 )
    {
        auto i = 0;
        dbg::File
        exit( -8 );
    }/**/

    if (open_list.size() != 0) {
        auto nodecost = (*open_list.begin())->f;
#ifdef _FRONT_SORT_
        if ( search_value <= nodecost )
        {

            open_list.insert( open_list.begin(), Cur );

            for ( auto N : open_list )
            {

            }
            return void( 0 );
        }
        nodecost = ( *( open_list.end() - 1 ) )->f;
        if ( search_value >= nodecost )
        {

            open_list.push_back( Cur );

            for ( auto N : open_list )
            {

            }
            return void( 0 );
        }
#else
        if (search_value >= nodecost) {
            open_list.insert(open_list.begin(), Cur);
            for (auto N : open_list) {

            }
            return void(0);
        }
        nodecost = (*(open_list.end() - 1))->f;
        if (search_value < nodecost) {
            open_list.push_back(Cur);
            for (auto N : open_list) {

            }
            return void(0);
        }
#endif
        unsigned int lo_index = 0, hi_index = open_list.size() - 1;
        int increment = (hi_index - lo_index) / 2;
        for (unsigned int pos = 0; true; pos += increment) {
            nodecost = (*(open_list.begin() + pos))->f;
#ifdef _FRONT_SORT_
            //Open List sorted for cheapest at the front
            //Sort expensive to the back
            if ( search_value > nodecost )
            {
                lo_index = pos;
                increment = ( ( hi_index - lo_index ) / 2 ) + 1;
            }
            else
            {
                hi_index = pos;
                increment = 0 - ( hi_index - lo_index ) / 2;
            }
#else
            //Open List sorted for cheapest at the back
            //Sort expensive to the front
            if (search_value < nodecost) {
                lo_index = pos;
                increment = ((hi_index - lo_index) / 2) + 1;
            } else {
                hi_index = pos;
                increment = 0 - ((hi_index - lo_index) / 2);
            }
#endif
            if (lo_index == hi_index || increment == 0) {
                pos += increment;
                open_list.insert(open_list.begin() + pos, Cur);
                for (auto N : open_list) {

                }
                return void(0);
            }
            if (infinite++ > 30) {
                exit(-8);
            }
        }
    } else {
        open_list.push_back(Cur);
    }
}

//Node must exist in Open_List
void pathfinder::binary_remove(node *Cur, float search_value) {
    for (auto N : open_list) {

    }
    if ((*(open_list.end() - 1)) == Cur) {
        open_list.erase(open_list.end() - 1);
        return void(0);
    } else if ((*open_list.begin()) == Cur) {
        open_list.erase(open_list.begin());
        return void(0);
    }
    unsigned int before = 0, after = open_list.size() - 1;
    int increment = (after - before) / 2;
    int pos;
    for (pos = 0; true; pos += increment) {
        auto the_node = *(open_list.begin() + pos);
        auto nodecost = the_node->f;
#ifdef _FRONT_SORT_
        if ( search_value > nodecost )
        {
            before = pos;
            increment = ( ( after - before ) / 2 )+1;
        }
        else if ( search_value < nodecost )
        {
            after = pos;
            increment = 0 - ( ( after - before ) / 2 );
        }
        else
        {

            for ( unsigned int i = before; i <= after; ++i )
            {
                auto iter = open_list.begin() + i;
                if ( *iter == Cur )
                {
                    open_list.erase( open_list.begin() + i );
                    return void( 0 );
                }
            }
            dbg::File
            exit( 404 );
        }
#else
        if (search_value < nodecost) {
            before = pos;
            increment = ((after - before) / 2) + 1;
        } else if (search_value > nodecost) {
            after = pos;
            increment = 0 - ((after - before) / 2);
        } else {
            for (unsigned int i = before; i <= after; ++i) {
                auto iter = open_list.begin() + i;
                if (*iter == Cur) {
                    open_list.erase(open_list.begin() + i);
                    return void(0);
                }
            }
            exit(404);
        }
#endif
        if (after == before) {
            the_node = *(open_list.begin() + pos);
            if (the_node == Cur) {
                open_list.erase(open_list.begin() + pos);
                return void(0);
            } else {
                exit(404);
            }
        } else if ((after - before) == 1) {
            exit(404);
        }
    }
}

void pathfinder::remove(node *Cur) {
    int i = 0;
    for (auto N : open_list) {
        if (N == Cur) {
            open_list.erase(open_list.begin() + i);
            return void(0);
        }
        ++i;
    }
}

void pathfinder::sync() {
    if (C1.joinable())
        C1.join();
}

void Path::TracePath(node *goal_node_found) {
    node *current = goal_node_found;
    while (current != nullptr) {
        route.push_back(current->pos);
        current = current->parent;
    }
}

void Path::ShowPath() {
    for (int i = route.size() - 1; i >= 0; --i) {
        coords pos = route[i];
        std::cout << " X: " << pos.first << "  Y: " << pos.second << "\t";
    }
    std::cout << std::endl << route.size() << std::endl;
}