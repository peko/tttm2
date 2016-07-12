
#include <math.h>

#include "text.h"

// A---B
// | / |
// C---D
// | / |
// E---F

#define W 2
#define H 3

#define A 0,2
#define B 1,2
#define C 0,1
#define D 1,1
#define E 0,0
#define F 1,0

#define D0 {5,A,B,F,E,A}
#define D1 {3,C,B,F}
#define D2 {5,A,B,D,E,F}
#define D3 {6,A,B,C,D,F,E}
#define D4 {5,A,C,D,B,F}
#define D5 {6,B,A,C,D,F,E}
#define D6 {6,B,A,E,F,D,C}
#define D7 {4,A,B,D,E}
#define D8 {7,C,D,F,E,A,B,D}
#define D9 {6,D,C,A,B,F,E}
#define MN {2,D,C}

uint8_t digits[10][15] = {D0,D1,D2,D3,D4,D5,D6,D7,D8,D9};

shapes_v
text_int(
    uint32_t n, 
    double x, 
    double y,
    double s) {

    shapes_v shapes;
    kv_init(shapes);
    
    uint8_t k = ceil(log10(n)); // digit count
    // printf("%d %d\n", n, k);
    // 123 -> 3, 2, 1
    uint8_t j = 1;
    while (n) {
        int d = n % 10; // digit id
        shape_v shape;
        kv_init(shape);        
        for(uint8_t i=0; i<digits[d][0];i++) {
            double o = k*W - W*j; // char offset
            point_t p = (point_t){
                x + (o + digits[d][i*2+1])*s,
                y + (    digits[d][i*2+2])*s
            };
            kv_push(point_t, shape, p);
        }
        j++;
        kv_push(shape_v, shapes, shape);        
        n /= 10;
    }
    return shapes;
}