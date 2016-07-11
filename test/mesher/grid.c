
#include "grid.h"

shapes_v 
grid_make(const point_t* min, const point_t* max) {
 
    int n = 10;

    double w = max->x - min->x;
    double h = max->y - min->y;
    double dx = w/n;
    double dy = h/n;

    shapes_v grid;
    kv_init(grid);

    // vertical
    for(int x=0; x<=n; x+=n) {
        shape_v line;
        kv_init(line);
        for(int y=0; y<=n*4; y++) {
            point_t p = (point_t){min->x+x*dx, min->y+y*dy*0.25}; 
            kv_push(point_t, line, p);
        }
        kv_push(shape_v, grid, line);
    }

    // horizontal
    for(int y=0; y<=n; y+=n) {
        shape_v line;
        kv_init(line);
        for(int x=0; x<=n*4; x++) {
            point_t p = (point_t){min->x+x*dx*0.25, min->y+y*dy}; 
            kv_push(point_t, line, p);
        }
        kv_push(shape_v, grid, line);
    }

    // vertical
    for(double x=ceil(min->x); x<=floor(max->x); x+=1.0) {
        shape_v line;
        kv_init(line);
        for(double y=floor(min->y); y<=ceil(max->y); y+=0.25) {
            point_t p = (point_t){x, y < min->y ? min->y : y > max->y ? max->y : y};
            kv_push(point_t, line, p);
        }
        kv_push(shape_v, grid, line);
    }

    // horizontal
    for(double y=ceil(min->y); y<=floor(max->y); y+=1.0) {
        shape_v line;
        kv_init(line);
        for(double x=floor(min->x); x<=ceil(max->x); x+=0.25) {
            point_t p = (point_t){x < min->x ? min->x : x > max->x ? max->x : x, y}; 
            kv_push(point_t, line, p);
        }
        kv_push(shape_v, grid, line);
    }

    return grid;

}
