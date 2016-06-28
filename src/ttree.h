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