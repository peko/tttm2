#pragma once

#include "types.h"

countries_v
shape_load_countries(const char* filename);

shapes_v
shape_proj(const shapes_v* shapes, const char* from, const char* to);

strings_v
shape_load_names(const char* filename, const char* colname);