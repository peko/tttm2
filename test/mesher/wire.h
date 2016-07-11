#pragma once

#include "linmath.h"
#include "types.h"
#include "vbo.h"


void wire_init();
void wire_set_buffer(shapes_v* shapes);
void wire_draw(vbo_t* vbo, vec3 col, float ratio, float x, float y, float scale);
void wire_cleanup();