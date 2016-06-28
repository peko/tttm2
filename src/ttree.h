#pragma once

#include "primitives.h"

typedef struct tnode_t tnode_t;
typedef struct tnode_t {
    triangle_t triangle;
    tnode_t* children[4];
} tnode_t;

typedef struct {
    points_v points;
    tnode_t* root;
} ttree_t;

ttree_t* ttree_new();
void ttree_free(ttree_t* tt);

ttree_t* ttree_from_points(const points_v p);
ttree_t* ttree_from_mesh(
    points_v*    points, 
    triangles_v* triangles, 
    uint8_t depth);

void
ttree_split_by_mesh(
    ttree_t*     tt,
    tnode_t*     tn,
    points_v*    points,
    triangles_v* triagles,
    uint8_t      depth);

void 
ttree_split_node(
    ttree_t* t, 
    tnode_t* n, 
    point_t* p);

void
ttree_write(
    ttree_t* tree, 
    FILE*    fp);

void
tnode_write(
    tnode_t* tn,
    ttree_t* tt,
    uint32_t d,
    FILE*    fp);