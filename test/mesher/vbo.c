#include "types.h"
#include "vbo.h"

vbo_t
vbo_new(shapes_v* shapes) {

    vbo_t vbo;
    vbo.parts = (uint32_t*) calloc(shapes->n, sizeof(uint32_t));

    // sum shapes points
    vbo.vertices = 0;
    vbo.shapes = shapes->n;
    for(uint32_t s=0; s<shapes->n; s++) {
        vbo.vertices += shapes->a[s].n;
        vbo.parts[s] = shapes->a[s].n;
    }

    // fill buffer
    vertex_t vertices[vbo.vertices];
    uint32_t i=0;
    for(uint32_t s=0; s<shapes->n; s++) {
        for(uint32_t p=0; p<shapes->a[s].n; p++) {
            point_t* pnt = &shapes->a[s].a[p];
            vertices[i++] = (vertex_t) {pnt->x, pnt->y};
        }
    }

    // send buffer to vram
    glGenBuffers(1, &vbo.id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo.id);    
    glBufferData(GL_ARRAY_BUFFER, 
        vbo.vertices * sizeof(vertex_t), 
        vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    return vbo;
}

void
vbo_destroy(vbo_t* vbo) {
    // clean up VRAM
    glDeleteBuffers(1, &vbo->id);
    // free parts
    free(vbo->parts);
}

