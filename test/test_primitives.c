#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "primitives.h"

void test_line_intersects(){
    point_t a1, b1, a2, b2;
    int d = 1000;
    FILE* fp = fopen("test_line_intersects.txt", "w");
    for(int i=0; i<100; i++) {
        a1 = (point_t){d/2-rand()%d,d/2-rand()%d}; 
        b1 = (point_t){d/2-rand()%d,d/2-rand()%d}; 
        a2 = (point_t){d/2-rand()%d,d/2-rand()%d}; 
        b2 = (point_t){d/2-rand()%d,d/2-rand()%d};
        bool intersects = lines_intersects(&a1, &b1, &a2, &b2);
        fprintf(fp, "%f %f %d\n%f %f %d\n", a1.x, a1.y, intersects, b1.x, b1.y, intersects);
        fprintf(fp, "\n");
        fprintf(fp, "%f %f %d\n%f %f %d\n", a2.x, a2.y, intersects, b2.x, b2.y, intersects);
        fprintf(fp, "\n");
    }
    fclose(fp);
}

int
main(int argc, char** argv) {

    point_t t[3];
    double r = 10.0;
    point_t o = (point_t){0, 0};
    point_t A = (point_t){r, r};

    triangle_by_incircle(t, o , r);
    printf("Triangle with incircle o(0,0) r:%4.2f:\n\t0 x:%4.2f y:%4.2f\n\t1 x:%4.2f y:%4.2f\n\t2 x:%4.2f y:%4.2f\n",
        r, t[0].x, t[0].y, t[1].x, t[1].y, t[2].x, t[2].y);

    bool cw = points_is_cw(t[0], t[1], t[2]);
    printf("Points is clockwise  true = %s\n", cw ? "true" : "false");
    
    bool ccw = points_is_ccw(t[0], t[1], t[2]);
    printf("Points is counterclocwise false = %s\n", ccw ? "true" : "false");
    
    printf("Point is inside cw true = %s\n", point_is_inside(o, t[0], t[1], t[2]) ? "true" : "false");
    printf("Point is inside ccw false = %s\n", point_is_inside(o, t[0], t[2], t[1]) ? "true" : "false");

    point_t b = point_to_barycentric(&o, &t[0], &t[1], &t[2]);
    printf("Baricentric o(0,0): \n\t u: %4.2f v: %4.2f w: %4.2f\n", b.x, b.y, 1-b.x-b.y);
    printf("Point is o(0,0) inside bc true = %s\n", point_is_inside_bc(&o, &t[0], &t[2], &t[1]) ? "true" : "false");
    printf("Point is A(r,r) inside bc false = %s\n", point_is_inside_bc(&A, &t[0], &t[2], &t[1]) ? "true" : "false");

    test_line_intersects();

    return 0;
}