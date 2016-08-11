
#include <stdio.h>
#include <assert.h>
#include <stdint.h>
#include <math.h>

#include "tttm.h"
#include "colors.h"

void 
shape_print_point(shape_t* s, uint32_t i) {
    assert(i < kv_size(s->points));
    point_t p = kv_A(s->points, i);
    printf("%u (%f, %f) ",  i, p.x, p.y);
}

void 
shape_print_loop(shape_t* s, uint32_t lid) {
    assert( lid < kv_size(s->loops) && s != NULL );
    loop_t l = kv_A(s->loops, lid);
    for(uint32_t i=0; i< kv_size(l); i++) {
        uint32_t pid = kv_A(l, i);
        shape_print_point(s, pid);
    }
    printf("\n");
}

void 
shape_print_loops(shape_t* s) {
    assert( s!=NULL );
    for(uint32_t lid=0; lid< kv_size(s->loops); lid++) {
        printf("[%u] ", lid);
        shape_print_loop(s, lid);
    }
}

void 
shape_print(shape_t* s) {
    printf(KBLU "points:\n"KNRM);
    for(uint32_t pid=0; pid< kv_size(s->points); pid++) {
        shape_print_point(s, pid);
        printf("%c", pid%4 ? ' ' : '\n');
    }
    printf("\n");
    printf(KBLU "loops:\n"KNRM);
    shape_print_loops(s);
}

// ----------------------------------------------------------------------------

void 
mesh_print_point(mesh_t* m, uint32_t i) {
    assert(i < kv_size(m->points));
    point_t p = kv_A(m->points, i);
    printf("%u (%f, %f) ",  i, p.x, p.y);
}

void 
mesh_print_triangle(mesh_t* m, uint32_t i) {
    assert(i < kv_size(m->triangles));
    triangle_t t = kv_A(m->triangles, i);
    mesh_print_point(m, t.a);
    mesh_print_point(m, t.b);
    mesh_print_point(m, t.c);
}

void 
mesh_print(mesh_t* m) {
    assert(m!=NULL);
    printf(KBLU "mesh\n"KNRM);
    printf(KBLU "points:"KNRM" %zu\n"   , kv_size(m->points)   );
    for(uint32_t i=0; i<kv_size(m->points); i++) {
        mesh_print_point(m, i);
        if(i%4) printf(" "); else printf("\n");
    }
    printf("\n");

    printf(KBLU "triangles:"KNRM" %zu\n", kv_size(m->triangles));
    for(uint32_t i=0; i<kv_size(m->triangles); i++) {
        mesh_print_triangle(m, i);
        printf("\n");
    }
}

// ----------------------------------------------------------------------------

void 
test_mesh() {

    printf(KGRN"TEST MESH\n"KNRM);

    mesh_t* m = mesh_new();

    uint32_t tid = mesh_add_triangle(m,
      (triangle_t){
        mesh_add_point(m, (point_t){0.0, 0.0}),
        mesh_add_point(m, (point_t){1.0, 0.0}),
        mesh_add_point(m, (point_t){0.0, 1.0}),
      }
    );
    printf(KBLU "points:"KNRM" %zu\n"   , kv_size(m->points)   );
    printf(KBLU "triangles:"KNRM" %zu\n", kv_size(m->triangles));
    mesh_print_triangle(m, tid);
    printf("\n");
    
    triangle_t t = kv_A(m->triangles, tid);
    uint32_t pid = mesh_add_point(m, (point_t){1.0, 1.0});

    tid = mesh_add_triangle(m, (triangle_t){ t.b, t.c, pid });
    printf(KBLU "points:"KNRM" %zu\n"   , kv_size(m->points)   );
    printf(KBLU "triangles:"KNRM" %zu\n", kv_size(m->triangles));
    mesh_print_triangle(m, tid);
    printf("\n");
    
    mesh_free(m);
}

void 
test_shape() {

    printf(KGRN"TEST SHAPE\n"KNRM);

    shape_t* s = shape_new();
    uint8_t points = 5;
    uint8_t loops  = 3;
    // teen contours in shape
    for(uint8_t j=0; j<loops; j++) {
        loop_t l;
        kv_init(l);
        for(uint8_t i=0; i<points; i++) {
            uint32_t pid = shape_add_point(s, (point_t){i+j*points,i+j*points} );
            kv_push(uint32_t, l, pid);
        }
        shape_add_loop(s, l);
    }
    shape_print(s);
    shape_free(s);

}

void 
test_partitioning() {
    
    printf(KGRN"TEST PARTITIONING\n"KNRM);

    shape_t* s = shape_new();
    loop_t l;
    kv_init(l);

    for(uint32_t i=0; i<36; i++) {
        point_t p = (point_t) {
            100 * cos(3.1415926*i/18.0),
            100 * sin(3.1415926*i/18.0)};
        uint32_t pid = shape_add_point(s, p);
        kv_push(uint32_t, l, pid);
    }
    kv_push(loop_t, s->loops, l);

    mesh_t* m = shape_triangulate(s);
    mesh_print(m);
    mesh_free(m);    
}


int 
main(int argc, char** argv) {
    test_mesh();
    test_shape();
    test_partitioning();
    return(0);
}