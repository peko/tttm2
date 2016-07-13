#pragma once

#include "types.h"


// load all countries in separate shapes_v
countries_v
shape_load_countries(const char* filename);

// load all countries in one merged shapes_v
shapes_v 
shape_load_globe(const char* filename);

// reprojects shape
shapes_v
shape_proj(const shapes_v* shapes, const char* from, const char* to);

// load column from dbf
strings_v
shape_load_names(const char* filename, const char* colname);