#pragma once

typedef struct ttnode_t;
typedef struct {
    triangle_t triangle;
    ttnode_t chidlren[4];
} ttnode_t;

typedef struct {
    ttnode_t root; 
} ttree_t;

ttree_t* ttree_new();
void ttree_free(ttree_t* tt);


