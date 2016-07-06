#pragma once

#include "types.h"

void wire_init();
void wire_set_buffer(shapes_v* shapes);
void wire_draw(float ratio, float x, float y, float scale);
void wire_cleanup();