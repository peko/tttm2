#pragma once

#include <stdint.h>

#include "point.h"
#include "shape.h"
#include "triangle.h"

typedef struct {
    kvec_t(point_t) points;
    kvec_t(triangle_t) triangles;
} mesh_t;

mesh_t* mesh_new();
void mesh_free(mesh_t* m);

mesh_t* mesh_from_shape(shape_t* s);
int8_t  mesh_from_poly(mesh_t* mesh, const points_v* points, const poly_t* poly);

void mesh_write(const mesh_t* m, FILE* fp);