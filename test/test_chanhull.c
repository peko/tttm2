#include <stdio.h>
#include <assert.h>
#include <string.h>

#include "shape.h"
#include "chanhull.h"

int
main(int argc, char** argv) {

    shapes_v* shapes = shapes_load_shp("../data/earth_110m.shp");
    shapes_v* shapes_pr = shapes_proj(shapes,
        "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs",
        "+proj=laea +lat_0=89.9 +lon_0=90 +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs"
    );

    shape_t* shape = kv_A(*shapes_pr, 135);

    points_v hull;
    kv_init(hull);
    kv_copy(point_t, hull, shape->points);
    int l = kv_size(hull);
    fprintf(stderr, "total points: %d\n", l);
    int s = chanhull(&kv_A(hull,0), l);
    fprintf(stderr, "start hull at: %d\n", s);

    fprintf(stderr, "write hull.txt\n");
    FILE* fp = fopen("hull.txt","w");
    for(int i=s; i<l; i++) {
        point_t* p = &kv_A(hull, i);
        fprintf(fp,"%f %f\n", p->x, p->y);
    }
    fclose(fp);
    
    fprintf(stderr, "write points.txt\n");
    fp = fopen("points.txt", "w");
    shape_write(shape, fp);
    fclose(fp);
    
    kv_destroy(hull);
    shapes_free(shapes_pr);
    shapes_free(shapes);

    return 0;
}
