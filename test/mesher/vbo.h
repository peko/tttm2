#pragma once

#include <stdint.h>

#include "glad/glad.h"
// #include <GLFW/glfw3.h>

#include "shape.h"

typedef struct {
    GLuint    id;
    uint32_t  shapes;
    uint32_t  vertices;
    uint32_t* parts;
} vbo_t;


vbo_t vbo_new(shapes_v* shapes);
void  vbo_destroy(vbo_t* vbo);