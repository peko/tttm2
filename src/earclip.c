#include <stdint.h>

#include "point.h"

static typedef struct part_t part_t;
static typedef struct part_t {
    bool is_active;
    bool is_convex;
    bool is_ear;
    point_t p;
    double angle;
    part_t* prev;
    part_t* next;
} part_t;

static void
update_part_point(
    part_t* v,
    part_t* points,
    uint32_t points_count) {

    point_part_t *v1, *v3;
    point_t vec1, vec3;

    v1 = v->prev;
    v3 = v->next;

    v->is_convex = points_is_convex(v1->p, v->p, v3->p);

    vec1 = points_norm(points_sub(v1->p, v->p));
    vec3 = points_norm(points_sub(v3->p, v->p));
    v->angle = vec1.x*vec3.x + vec1.y*vec3.y;

    if(v->is_convex) {
        v->is_ear = true;
        for(uint32_t j=0; j<points_count; j++) {
            if((points[j].p.x ==  v->p.x) && (points[j].p.y ==  v->p.y)) continue;
            if((points[j].p.x == v1->p.x) && (points[j].p.y == v1->p.y)) continue;
            if((points[j].p.x == v3->p.x) && (points[j].p.y == v3->p.y)) continue;
            if(point_is_inside(v1->p, v->p, v3->p, points[j].p)) {
                v->is_ear = false;
                break;
            }
        }
    } else {
        v->is_ear = false;
    }
}

mesh_t*
earclip(shape_t* s) {

    uint32_t points_count = kv_size(s->points);

    if(points_count < 3) return NULL;

    mesh_t* m = mesh_new();
    if(points_count == 3) {
        mesh_add_triangle(m, (triangle_t) {
            mesh_add_point(m, kv_A(s->points, 0)),
            mesh_add_point(m, kv_A(s->points, 1)),
            mesh_add_point(m, kv_A(s->points, 2)),
        });
        return m;
    }
    point_part_t points[points_count];
    for(uint32_t i=0; i<points_count; i++) {
        points[i].is_active = true;
        points[i].p = kv_A(s->points, i);
        if(i==points_count-1) points[i].next = &(points[  0]);
        else                  points[i].next = &(points[i+1]);
        if(i==0)              points[i].prev = &(points[points_count-1]);
        else                  points[i].prev = &(points[i-1]);
    }

    for(uint32_t i=0; i<points_count; i++) {
        update_part_point(&points[i], points, points_count);
    }

    for(uint32_t i=0; i<points_count-3; i++) {

        point_part_t* ear;
        bool earfound = false;

        //find the most extruded ear
        for(uint32_t j=0;j<points_count;j++) {
            if(!points[j].is_active) continue;
            if(!points[j].is_ear   ) continue;
            if(!earfound) {
                earfound = true;
                ear = &(points[j]);
            } else {
                if(points[j].angle > ear->angle) {
                    ear = &(points[j]);
                }
            }
        }

        if(!earfound) {
            mesh_free(m);
            return NULL;
        }
        
        mesh_add_triangle(m, (triangle_t) {
            mesh_add_point(m, ear->prev->p),
            mesh_add_point(m, ear->p      ),
            mesh_add_point(m, ear->next->p),
        });

        ear->is_active = false;
        ear->prev->next = ear->next;
        ear->next->prev = ear->prev;

        if(i==points_count-4) break;

        shape_update_part_point(ear->prev, points, points_count);
        shape_update_part_point(ear->next, points, points_count);
    }

    for(uint32_t i=0; i<points_count; i++) {
        if(points[i].is_active) {
            // triangle.Triangle(points[i].prev->p,points[i].p,points[i].next->p);
            // triangles->push_back(triangle);
            mesh_add_triangle(m, (triangle_t) {
                mesh_add_point(m, points[i].prev->p),
                mesh_add_point(m, points[i].p      ),
                mesh_add_point(m, points[i].next->p),
            });
            break;
        }
    }

    return m;
}
