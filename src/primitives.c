#include "primitives.h"

/////////////
// POINT_T //
/////////////

/*
Barycentric x axis for a point
   
          a       1    h = sqrt(3)/2
         / \      |
        /   \    2/3
       *-----*   1/2
      / \ o / \  1/3
     /   \ /   \  |
    c-----*-----b 0
*/

point_t
point_to_barycentric(
    const point_t* p,
    const point_t* a, 
    const point_t* b, 
    const point_t* c){
    
    point_t d1 = (point_t) {a->x - c->x, a->y - c->y};
    point_t d2 = (point_t) {b->x - c->x, b->y - c->y};
    point_t d3 = (point_t) {p->x - c->x, p->y - c->y};
    
    double k = (d2.y*d1.x-d2.x*d1.y);
    double u = (d2.y*d3.x-d2.x*d3.y)/k;
    double v = (d1.x*d3.y-d1.y*d3.x)/k;
    
    return (point_t) {u, v};
}

// converts an point p(u,w) 
// from an baricentric triangle space abc
// to cartesian space
point_t
point_to_cartesian(
    const point_t* p,
    const point_t* a,
    const point_t* b,
    const point_t* c){

    double w = 1 - p->x - p->y;
    return (point_t) {
        a->x * p->x + b->x * p->y + c->x*w,
        a->y * p->x + b->y * p->y + c->y*w};
}

bool
point_is_inside_bc(
    const point_t* p,
    const point_t* a,
    const point_t* b,
    const point_t* c) {

    point_t bc = point_to_barycentric(p, a, b, c);
    return (bc.x >= 0 && bc.y >= 0 && bc.x+bc.y<=1) ? true : false;
}

//////////////
// TRIANGLE //
//////////////

triangle_t
triangle_new(
    points_v* p, 
    point_t*  a,
    point_t*  b,
    point_t*  c) {

    uint32_t s = p->n;
    kv_push(point_t, *p, *a);
    kv_push(point_t, *p, *b);
    kv_push(point_t, *p, *c);

    return (triangle_t) {s, s+1, s+2};
}

void
triangle_by_incircle(point_t t[3], point_t o, double r){
    t[0].x = o.x          ; t[0].y = o.y + r*2;
    t[1].x = o.x + r*SQRT3; t[1].y = o.y - r  ;
    t[2].x = o.x - r*SQRT3; t[2].y = o.y - r  ;
}

bool 
lines_intersects(
    const point_t* a1, 
    const point_t* b1,
    const point_t* a2,
    const point_t* b2) {

    double d, u, v;

    d = (b2->y - a2->y) * (b1->x - a1->x) - 
        (b2->x - a2->x) * (b1->y - a1->y);
    u = (b2->x - a2->x) * (a1->y - a2->y) - 
        (b2->y - a2->y) * (a1->x - a2->x);
    v = (b1->x - a1->x) * (a1->y - a2->y) - 
        (b1->y - a1->y) * (a1->x - a2->x);

    if(d<0) {
        u = -u;
        v = -v;
        d = -d;
    }

    return 0<=u && u<=d && 0<=v && v<=d;
}

bool 
triangles_intersects(
    const point_t* a1, const point_t* b1, const point_t* c1, 
    const point_t* a2, const point_t* b2, const point_t* c2) {

    if(lines_intersects(a1, b1, a2, b2)) return true;
    if(lines_intersects(a1, b1, b2, c2)) return true;
    if(lines_intersects(a1, b1, c2, a2)) return true;
    
    if(lines_intersects(a1, c1, a2, b2)) return true;
    if(lines_intersects(a1, c1, b2, c2)) return true;
    if(lines_intersects(a1, c1, c2, a2)) return true;

    if(point_is_inside_bc(a2, a1, b1, c1)) return true;
    if(point_is_inside_bc(b2, a1, b1, c1)) return true;
    if(point_is_inside_bc(c2, a1, b1, c1)) return true;
    
    if(point_is_inside_bc(a2, a2, b2, c2)) return true;
    if(point_is_inside_bc(b2, a2, b2, c2)) return true;
    if(point_is_inside_bc(c2, a2, b2, c2)) return true;

    return false;
}

