//
// Created by joseph on 11/20/24.
//

#include "quadtree.h"

//insert a new element to the tree
void quadtree::insert(state *el) {
    //sanity checks
    assert(el != nullptr);
    assert(el != this_el);
    assert(el->latitude != this_el->latitude && el->longitude != this_el->longitude);

    if (el->latitude < this_el->latitude) {
        //top half
        if (el->longitude < this_el->longitude) {
            // left upper quadrant
            if (nw == nullptr) {
                //new leaf
                nw = new quadtree(this, el);
            } else if (el->latitude > nw->this_el->latitude && el->longitude < nw->this_el->longitude) {
                //if the leaf is closer to the root than us, recursively call insert
                nw->insert(el);
            } else {
                //insert in middle of tree
                quadtree *tmp_nw = nw;
                nw = new quadtree(predecessor, el);
                nw->insert(tmp_nw);
            }
        } else {
            //right upper quadrant
            if (ne == nullptr) {
                //new leaf
                ne = new quadtree(this, el);
            } else if (el->latitude > ne->this_el->latitude && el->longitude > ne->this_el->longitude) {
                //if the leaf is closer to the root than us, recursively call insert
                ne->insert(el);
            } else {
                //insert in middle of tree
                quadtree *tmp_ne = ne;
                ne = new quadtree(predecessor, el);
                ne->insert(tmp_ne);
            }
        }
    } else {
        //bottom half
        if (el->longitude < this_el->longitude) {
            //lower left quadrant
            if (sw == nullptr) {
                // new leaf
                sw = new quadtree(this, el);
            } else if (el->latitude < sw->this_el->latitude && el->longitude < sw->this_el->longitude) {
                //if the leaf is closer to the root than us, recursively call insert
                sw->insert(el);
            } else {
                //insert in middle of tree
                quadtree *tmp_sw = sw;
                sw = new quadtree(predecessor, el);
                sw->insert(tmp_sw);
            }
        } else {
            if (se == nullptr) {
                //new leaf
                se = new quadtree(this, el);
            } else if (el->latitude < se->this_el->latitude && el->longitude > se->this_el->longitude) {
                //if the leaf is closer to the root than us, recursively call insert
                se->insert(el);
            } else {
                //insert in middle of tree
                quadtree *tmp_se = se;
                se = new quadtree(predecessor, el);
                se->insert(tmp_se);
            }
        }
    }
}

void quadtree::insert(quadtree *subtree) {
    assert(subtree != nullptr);

    if (subtree->this_el->latitude < this_el->latitude) {
        //top half
        if (subtree->this_el->longitude < this_el->longitude) {
            // left upper quadrant
            if (nw == nullptr) {
                //new leaf
                subtree->predecessor = this;
                nw = subtree;
            } else if (subtree->this_el->latitude > nw->this_el->latitude && subtree->this_el->longitude < nw->this_el->
                       longitude) {
                //if the leaf is closer to the root than us, recursively call insert
                nw->insert(subtree);
            } else {
                //insert in middle of tree
                quadtree *tmp_nw = nw;
                subtree->predecessor = this;
                subtree->insert(tmp_nw);
                nw = subtree;
            }
        } else {
            //right upper quadrant
            if (ne == nullptr) {
                //new leaf
                subtree->predecessor = this;
                ne = subtree;
            } else if (subtree->this_el->latitude > ne->this_el->latitude && subtree->this_el->longitude > nw->this_el->
                       longitude) {
                //if the leaf is closer to the root than us, recursively call insert
                ne->insert(subtree);
            } else {
                //insert in middle of tree
                quadtree *tmp_ne = ne;
                subtree->predecessor = this;
                subtree->insert(tmp_ne);
                ne = subtree;
            }
        }
    } else {
        //bottom half
        if (subtree->this_el->longitude < this_el->longitude) {
            //lower left quadrant
            if (sw == nullptr) {
                // new leaf
                subtree->predecessor = this;
                sw = subtree;
            } else if (subtree->this_el->latitude < sw->this_el->latitude && subtree->this_el->longitude < sw->this_el->
                       longitude) {
                //if the leaf is closer to the root than us, recursively call insert
                sw->insert(subtree);
            } else {
                //insert in middle of tree
                quadtree *tmp_sw = sw;
                subtree->predecessor = this;
                subtree->insert(tmp_sw);
                sw = subtree;
            }
        } else {
            if (se == nullptr) {
                //new leaf
                subtree->predecessor = this;
                se = subtree;
            } else if (subtree->this_el->latitude < se->this_el->latitude && subtree->this_el->longitude > se->this_el->
                       longitude) {
                //if the leaf is closer to the root than us, recursively call insert
                se->insert(subtree);
            } else {
                //insert in middle of tree
                quadtree *tmp_se = se;
                subtree->predecessor = this;
                subtree->insert(tmp_se);
                se = subtree;
            }
        }
    }
}
