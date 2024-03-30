#ifndef _A_STAR_H_
#define _A_STAR_H_

#define _MAP_CLOSED_LIST
//#define _DEQUE_OPEN_LIST
#ifdef _DEQUE_OPEN_LIST
#define _FRONT_SORT_
#define _INSERT_
#else
#define _INSERT_
#ifdef _INSERT_
//#define _FRONT_SORT_
#endif
#endif


#include "thebuilder.h"
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <deque>
#include <thread>

struct node {
    bool is_on_openlist = false;
    float f;
    float g;
    float h;

    coords pos;
    node *parent = nullptr;

    short &x = pos.first;
    short &y = pos.second;

    node() {
        init();
    }

    node(unsigned int x, unsigned int y) {
        init();
        this->x = x;
        this->y = y;
    }

    node(coords pos) {
        init();
        this->pos = pos;
    }

    node(coords pos, node *parent) {
        init();
        this->pos = pos;
        this->parent = parent;
    }

    node(coords pos, float f, float g, float h) {
        this->pos = pos;
        this->f = f;
        this->g = g;
        this->h = h;
    }

    node(coords pos, node *parent, float f, float g, float h) {
        this->pos = pos;
        this->parent = parent;
        this->f = f;
        this->g = g;
        this->h = h;
    }

    node(node &obj) {
        f = obj.f;
        g = obj.g;
        h = obj.h;
        x = obj.x;
        y = obj.y;
    }
    //~node()
    //{
    //	if ( parent != nullptr )
    //	{
    //		delete *parent;
    //		parent = nullptr;
    //	}
    //}
private:
    void init() {
        x = y = 0;
        f = g = h = 0;
        parent = nullptr;
    }
};

class Path {
private:
    std::vector<coords> route;
protected:
public:
    void TracePath(node *goal_node_found);

    void ShowPath();
};

class pathfinder {
private:
    char **themap = nullptr;
    node *current;

    maze_specs size;
    coords start;
    coords goal;
    float heuristic = 1.0f;
    std::thread C1;

#ifdef _MAP_CLOSED_LIST
    std::unordered_map<unsigned int, node *> closed_list;
#else
    std::vector<node*> closed_list;
#endif
#ifndef _DEQUE_OPEN_LIST
    std::vector<node *> open_list;
#else
    std::deque<node*> open_list;
#endif

    void sort();

protected:
    inline unsigned int hash(coords pos);

    unsigned int get_goal_distance(coords pos);

    bool canMove(coords pos);

    void cleanUp();

public:
    pathfinder();

    ~pathfinder() { sync(); }

    void setmap(char **map, maze_specs size);

    Path *search(coords start, const coords goal);

    inline void expand_node(node *current);

    inline void evaluate_expansion(coords Expansion);

    inline void sorted_insert(node *Cur, float f);

    inline void binary_remove(node *Cur, float search_value);

    inline void remove(node *Cur);

    void sync();
};

#endif