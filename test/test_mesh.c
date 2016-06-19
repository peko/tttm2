#include <stdio.h>
#include <assert.h>

#include "mesh.h"


int
main(int argc, char** argv) {

    
    shapes_v* shapes = shapes_load_shp("../data/earth_110m.shp");
    shapes_v* shapes_pr = shapes_proj(shapes,
        "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs",
        "+proj=laea +lat_0=89.9 +lon_0=90 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
    );

    shape_t* shape = kv_A(*shapes_pr, 135);
    fprintf(stderr, "shape points: %zu\n", kv_size(shape->points));
    fprintf(stderr, "shape parts:  %zu\n", kv_size(shape->polys));
    
    mesh_t* mesh = mesh_from_shape(shape);
    fprintf(stderr, "Write mesh.txt\n");
    FILE* fp = fopen("mesh.txt", "w");
    mesh_write(mesh, fp);
    fclose(fp);
    
    // print mesh for gnuplot
    // plot 'noears.dat' using 1:2 with lines 

    mesh_free(mesh);
    shapes_free(shapes_pr);
    shapes_free(shapes);

    return 0;
}