#include <stdint.h>

#include "point.h"
#include "mesh.h"
#include "point_t"

typedef struct point_part_t point_part_t;
typedef struct point_part_t {
    bool is_active;
    bool is_convex;
    bool is_ear;
    point_t p;
    double angle;
    point_part_t* prev;
    point_part_t* next;
} point_part_t;

mesh_t* triangulate(shape_t* s);
