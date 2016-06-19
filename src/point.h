#pragma once

#include <stdbool.h>
#include <math.h>

#include "kvec.h"

/* Left-turn, right-turn and collinear predicates */
#define area(a, b, c) (((b).x-(a).x)*((c).y-(a).y) \
                     - ((b).y-(a).y)*((c).x-(a).x) )
#define right_turn(a, b, c) (area(a, b, c) <  0)
#define left_turn(a, b, c)  (area(a, b, c) >  0)
#define collinear(a, b, c)  (area(a, b, c) == 0)

/* Macros for lexicographic comparison of two points */
#define sign(x) (((x) < 0) ? -1 : (((x) > 0) ? 1 : 0))
#define cmp(a, b) (((a).x == (b).x) ? sign((a).y-(b).y) : sign((a).x-(b).x))

typedef struct {
    double  x, y;
} point_t;

typedef kvec_t(point_t) points_v;

point_t point_sum(const point_t* p1, const point_t* p2); // sum
point_t point_dif(const point_t* p1, const point_t* p2); // difference
point_t point_nrm(const point_t* p);

void point_add(point_t* p1, const point_t* p2); // add
void point_sub(point_t* p1, const point_t* p2); // substract

bool point_is_convex(const point_t* p1, const point_t* p2, const point_t* p3);
bool point_is_inside(const point_t* p1, const point_t* p2, const point_t* p3, const point_t* p);
