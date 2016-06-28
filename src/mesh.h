#pragma once

#include <stdint.h>

#include "primitives.h"
#include "shape.h"

typedef struct {
    points_v points;
    triangles_v triangles;
} mesh_t;

mesh_t* mesh_new();
void mesh_free(mesh_t* m);

mesh_t* mesh_from_shape(const shape_t* s);
mesh_t* mesh_from_hull(const shape_t* s);
mesh_t* mesh_load_triangle(char* filename);
int8_t  mesh_poly(mesh_t* mesh, const point_t* p, const uint32_t l);

void mesh_write(const mesh_t* m, FILE* fp);