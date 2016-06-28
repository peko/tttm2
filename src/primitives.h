#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <math.h>

#include "kvec.h"

/////////////
// POINT_T //
/////////////

typedef struct {
    double  x, y;
} point_t;

typedef kvec_t(point_t) points_v;

/* Left-turn, right-turn and collinear predicates */
// #define points_crs(a, b) ((a).x*(b).y-(a).y*(b).x)
#define points_crs(a, b, c) (((b).x-(a).x)*((c).y-(a).y) - ((b).y-(a).y)*((c).x-(a).x))
#define points_dot(a, b) ((a).x*(b).x+(a).y*(b).y)
#define points_is_cw(a, b, c) (points_crs(a, b, c) <  0)
#define points_is_ccw(a, b, c) (points_crs(a, b, c) >  0)
#define points_is_collinear(a, b, c) (points_crs(a, b, c) == 0)
#define point_is_inside(p, a, b, c) ((\
    points_is_cw(a, p, b) || \
    points_is_cw(b, p, c) || \
    points_is_cw(c, p, a)) ? false : true)

/* Macros for lexicographic comparison of two points */
#define sign(x) (((x) < 0) ? -1 : (((x) > 0) ? 1 : 0))
#define points_cmp(a, b) (((a).x == (b).x) ? sign((a).y-(b).y) : sign((a).x-(b).x))

#define points_sum(a, b) ((point_t){(a).x+(b).x, (a).y+(b).y})
#define points_dif(a, b) ((point_t){(a).x-(b).x, (a).y-(b).y})
#define point_mag(a) sqrt((a).x*(a).x+(a).y*(a).y)
#define point_nrm(a) ((point_t){(a).x/point_mag(a), (a).y/point_mag(a)})
#define point_mid(a, b) ((point_t){((a).x+(b).x)/2.0, ((a).y+(b).y)/2.0})

// point_t point_sum(const point_t* p1, const point_t* p2); // sum
// point_t point_dif(const point_t* p1, const point_t* p2); // difference
// point_t point_nrm(const point_t* p);

// void point_add(point_t* p1, const point_t* p2); // add
// void point_sub(point_t* p1, const point_t* p2); // substract

// bool point_is_convex(const point_t* p1, const point_t* p2, const point_t* p3);
// bool point_is_inside(const point_t* p1, const point_t* p2, const point_t* p3, const point_t* p);

// baricentric
point_t 
point_to_barycentric(
    const point_t* p, 
    const point_t* a, 
    const point_t* b, 
    const point_t* c);

point_t 
point_to_cartesian(
    const point_t* p, 
    const point_t* a, 
    const point_t* b, 
    const point_t* c);

bool 
point_is_inside_bc(
    const point_t* p, 
    const point_t* a, 
    const point_t* b, 
    const point_t* c);

////////////////
// TRIANGLE_T //
////////////////

// r = a * sqrt(3)/6 
// R = a * sqrt(3)/3
// h = a * sqrt(3)/2

#define SQRT3 1.73205080757

// indexes of points in points_v
typedef struct {
    uint32_t a, b, c;
} triangle_t;
typedef kvec_t(triangle_t) triangles_v;

triangle_t
triangle_new(
    points_v* p, 
    point_t*  a,
    point_t*  b,
    point_t*  c);

void 
triangle_by_incircle(point_t p[3], point_t o, double r);

bool 
triangles_intersects(
    point_t* a1, point_t* b1, point_t* c1, 
    point_t* a2, point_t* b2, point_t* c2);