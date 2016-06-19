#include "point.h"

point_t
point_sum(const point_t* p1, const point_t* p2) {
    return (point_t) {p1->x + p2->x, p1->y + p2->y};
}

point_t
point_dif(const point_t* p1, const point_t* p2) {
    return (point_t) {p1->x-p2->x, p1->y-p2->y};
}

void
point_add(point_t* p1, const point_t* p2) {
    p1->x+=p2->x;
    p1->y+=p2->y;
}

void
point_sub(point_t* p1, const point_t* p2) {
    p1->x-=p2->x;
    p1->y-=p2->y;
}

// z magnitude of cross product
// aka cw or ccw direction
bool
point_is_convex(
    const point_t* p1,
    const point_t* p2,
    const point_t* p3) {

    return (
        (p3->y - p1->y) * (p2->x - p1->x) -
        (p3->x - p1->x) * (p2->y - p1->y))<0;
}

point_t
point_nrm(const point_t* p) {
    double l = sqrt(p->x*p->x + p->y*p->y);
    if (l==0) return (point_t){0, 0};
    else      return (point_t){p->x/l, p->y/l};
}

bool
point_is_inside(
    const point_t* p1,
    const point_t* p2,
    const point_t* p3,
    const point_t* p) {

    if(point_is_convex(p1, p, p2) ||
       point_is_convex(p2, p, p3) ||
       point_is_convex(p3, p, p1)) return false;
    else return true;
}
