//
// Created by joseph on 11/20/24.
//

#include "quadtree.h"

//insert a new element to the tree
void quadtree::insert(state *el) {
    //sanity checks
    assert(el != nullptr);
    assert(el != this_el);
    if (el->latitude != this_el->latitude && el->longitude != this_el->longitude) {
        throw std::runtime_error("quadtree::insert: latitude and longitude already exist");
    }

    if (el->latitude > this_el->latitude) {
        //top half
        if (el->longitude < this_el->longitude) {
            // left upper quadrant
            if (nw == nullptr) {
                //new leaf
                nw = new quadtree(this, el);
            } else {
                //if the leaf is closer to the root than us, recursively call insert
                nw->insert(el);
            }
        } else {
            //right upper quadrant
            if (ne == nullptr) {
                //new leaf
                ne = new quadtree(this, el);
            } else {
                //if the leaf is closer to the root than us, recursively call insert
                ne->insert(el);
            }
        }
    } else {
        //bottom half
        if (el->longitude < this_el->longitude) {
            //lower left quadrant
            if (sw == nullptr) {
                // new leaf
                sw = new quadtree(this, el);
            } else {
                //if the leaf is closer to the root than us, recursively call insert
                sw->insert(el);
            }
        } else {
            if (se == nullptr) {
                //new leaf
                se = new quadtree(this, el);
            } else {
                //if the leaf is closer to the root than us, recursively call insert
                se->insert(el);
            }
        }
    }
}


