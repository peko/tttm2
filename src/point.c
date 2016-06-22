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

//    0
//   / \
//  / 3 \
// 1-----2

point_t
point_to_baricentric(point_t p[4]){
    
    point_t d[3];
    d[0] = (point_t) {p[0].x-p[2].x, p[0].y-p[2].y};
    d[1] = (point_t) {p[1].x-p[2].x, p[1].y-p[2].y};
    d[2] = (point_t) {p[3].x-p[2].x, p[3].y-p[2].y};
    
    double k = (d[1].y*d[0].x-d[1].x*d[0].y);
    double u = (d[1].y*d[2].x-d[1].x*d[2].y)/k;
    double v = (d[0].x*d[2].y-d[0].y*d[2].x)/k;
    
    return (point_t) {u, v};
}

point_t
point_to_cartesian (point_t p[4]){

    double w = 1 - p[3].x - p[3].y;
    return (point_t) {
        p[0].x*p->x + p[1].x*p->y + p[2].x*w,
        p[0].y*p->x + p[1].y*p->y + p[2].y*w};
}
