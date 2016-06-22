#pragma once

typedef struct ttnode_t;
typedef struct {
    triangle_t triangle;
    ttnode_t* chidlren[4];
} ttnode_t;

typedef struct {
    points_v points;
    ttnode_t* root;
} ttree_t;

ttree_t* ttree_new();
void ttree_free(ttree_t* tt);


ttree_t* ttree_from_points(const points_v p);

