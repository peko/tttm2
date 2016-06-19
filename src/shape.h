#pragma once

#include <stdint.h>
#include <stdbool.h>

#include "point.h"
#include "kvec.h"

///////////
// SHAPE // 
///////////

// part of points vector in shape
typedef struct {
    uint32_t s; //start point
    uint32_t l; //number of points
} poly_t;
typedef kvec_t(poly_t) polys_v;

typedef struct {
    point_t min;
    point_t max;
} bbox_t;

typedef struct {
    points_v points;
    points_v hull;
    polys_v  polys;
    bbox_t   bbox;
} shape_t;

shape_t* shape_new();
void     shape_free(shape_t* shape);
shape_t* shape_copy(const shape_t* shape);

// shape_t* shape_from_esri_file(char* file);
void     shape_build_hull(shape_t* shape);
shape_t* shape_proj(const shape_t* shape, const char* from, const char* to);
uint32_t shape_add_point(shape_t* s, point_t p);
uint32_t shape_add_poly (shape_t* s, poly_t l);

void shape_write(const shape_t* shape, FILE* fp);
void shape_write_hull(const shape_t* shape, FILE* fp);

////////////
// SHAPES // 
////////////

typedef kvec_t(shape_t*) shapes_v;
shapes_v* shapes_new();
void shapes_free(shapes_v* shapes);
uint32_t shapes_add_shape(shapes_v* shapes, shape_t* shape);
shapes_v* shapes_load(const char* name);
shapes_v* shapes_load_shp(const char* filename);
void      shapes_load_dbf(const char* filename);
shapes_v* shapes_proj(const shapes_v* shapes, const char* from, const char* to);