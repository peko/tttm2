#include <stdio.h>

#include "shape.h"

int 
main(int argc, char** argv){

    FILE* fp;

    // shapes_load_dbf("../data/ne_10m_admin_0_countries.dbf");
    // return 0;
    
    shapes_v* shapes    = shapes_load_shp("../data/earth_110m.shp");
    shapes_v* shapes_pr = shapes_proj(shapes,
        "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs",
        "+proj=laea +lat_0=89.9 +lon_0=90 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
    );

    fprintf(stderr, "write russia-wgs.txt\n");
    fp = fopen("russia.txt", "w");
    shape_write(kv_A(*shapes, 135), fp);
    fclose(fp);
    
    fprintf(stderr, "write russia-laea.txt\n");
    fp = fopen("russia-laea.txt", "w");
    shape_write(kv_A(*shapes_pr, 135), fp);
    fclose(fp);

    // direct access
    shape_t* s = shapes_pr->a[135];
    shape_build_hull(s);
    
    fprintf(stderr, "write russia-laea-hull.txt\n");
    fp = fopen("russia-laea-hull.txt", "w");
    shape_write_hull(s, fp);
    fclose(fp);

    fprintf(stderr, "gnuplot> plot 'russia-laea-hull.txt' with lines, 'russia-laea.txt'with lines");

    shapes_free(shapes_pr);
    shapes_free(shapes);
}
