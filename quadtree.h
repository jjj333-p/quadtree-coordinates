/*
* Code by Joseph Winkie for CS321 on 12/02/2024
* This is the implementation of a quadtree.
* Each "node" is considered a subtree and contains all the
* quadtree data and code for its subtree. It relies on the
* state datatype to function as that holds the actual element
* of the node.
*/

#ifndef QUADTREE_H
#define QUADTREE_H
#include <cassert>
#include "state.h"

// template<class T>
class quadtree {
public:
    //init in constructor, null if it is the root
    quadtree *predecessor;

    //init on insert function
    quadtree *nw = nullptr;
    quadtree *ne = nullptr;
    quadtree *sw = nullptr;
    quadtree *se = nullptr;

    //nested element, i guess for convenience
    state *this_el;

    // //coords
    // float latitude;
    // float longitude;
    //
    // //name of state
    // std::string name;

    // //idk
    // quadtree() : predecessor(nullptr), this_el(nullptr) {
    // }

    //root node
    explicit quadtree(state *el) : predecessor(nullptr), this_el(el) {
        assert(el != nullptr);
    }

    //child node
    quadtree(quadtree *pre, state *el) : predecessor(pre), this_el(el) {
        assert(el != nullptr);
    }

    //recursive destructing
    ~quadtree() {
        delete nw;
        delete ne;
        delete sw;
        delete se;
        delete this_el;
    }

    [[nodiscard]] bool has_children() const {
        return nw == nullptr && ne == nullptr && sw == nullptr && se == nullptr;
    }

    //insert a new element to the tree
    void insert(state *el);

private:
    // T *root;
};


#endif //QUADTREE_H
