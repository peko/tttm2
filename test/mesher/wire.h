#pragma once

#include "types.h"

void wire_init(int argc, char** argv);
void wire_draw(shapes_v shapes, float ratio, float x, float y, float scale);
void wire_cleanup();