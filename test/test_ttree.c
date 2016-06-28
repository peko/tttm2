#include <stdio.h>
#include <assert.h>

#include "shape.h"
#include "ttree.h"


int
main(int argc, char** argv) {

    FILE* fp;
    
    // shapes_v* shapes = shapes_load_shp("../data/ne_10m_admin_0_countries.shp");
    shapes_v* shapes = shapes_load_shp("../data/earth_110m.shp");
    shapes_v* shapes_pr = shapes_proj(shapes,
        "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs",
        "+proj=laea +lat_0=89.9 +lon_0=90 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
    );

    // shape_t* shape = kv_A(*shapes_pr, 92);
    shape_t* shape = kv_A(*shapes_pr, 135);
    
    fprintf(stderr, "Write ttree-shape.txt\n");
    fp = fopen("ttree-shape.txt", "w");
    shape_write(shape, fp);
    fclose(fp);

    ttree_t* tree = ttree_from_points(shape->points);
    fprintf(stderr, "Write ttree-mesh.txt\n");
    fp = fopen("ttree-mesh.txt", "w");
    ttree_write(tree, fp);
    fclose(fp);
    ttree_free(tree);

    shapes_free(shapes_pr);
    shapes_free(shapes);

    return 0;
}