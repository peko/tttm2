#include <stdio.h>
#include <assert.h>

#include "mesh.h"


int
main(int argc, char** argv) {

    FILE* fp;
    
    // shapes_v* shapes = shapes_load_shp("../data/ne_10m_admin_0_countries.shp");
    shapes_v* shapes = shapes_load_shp("../data/earth_110m.shp");
    shapes_v* shapes_pr = shapes_proj(shapes,
        "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs",
        "+proj=laea +lat_0=89.9 +lon_0=90 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
    );

    shape_t* shape = kv_A(*shapes_pr, 135);
    shape_build_hull(shape);

    fprintf(stderr, "shape points: %zu\n", kv_size(shape->points));
    fprintf(stderr, "shape parts:  %zu\n", kv_size(shape->polys));
    fprintf(stderr, "hull points:  %zu\n", kv_size(shape->hull));
    

    fprintf(stderr, "Write shape.poly\n");
    

    fp = fopen("shape.poly", "w");
    shape_write_poly(shape, fp);
    fclose(fp); 

    fprintf(stderr, "Write shape.txt\n");
    fp = fopen("shape.txt", "w");
    shape_write(shape, fp);
    fclose(fp);    

    fprintf(stderr, "Write hull.txt\n");
    fp = fopen("hull.txt", "w");
    shape_write_hull(shape, fp);
    fclose(fp);    

    mesh_t* mesh = mesh_from_shape(shape);
    fprintf(stderr, "Write shape-mesh.txt\n");
    fp = fopen("shape-mesh.txt", "w");
    mesh_write(mesh, fp);
    fclose(fp);
    mesh_free(mesh);

    mesh = mesh_from_hull(shape);
    fprintf(stderr, "Write hull-mesh.txt\n");
    fp = fopen("hull-mesh.txt", "w");
    mesh_write(mesh, fp);
    fclose(fp);
    mesh_free(mesh);

    mesh_t* triagnle = mesh_load_triangle("shape.1");
    if(triagnle != NULL) {
        fprintf(stderr, "Write triagnle-mesh.txt\n");
        fp = fopen("triangle-mesh.txt", "w");
        mesh_write(triagnle, fp);
        fclose(fp);
        mesh_free(triagnle);
    } else {   
        fprintf(stderr, "Triangle's ele and node files can be produced:\n\t./triangle -pzq31.5 shape.poly");
    }

    shapes_free(shapes_pr);
    shapes_free(shapes);

    return 0;
}