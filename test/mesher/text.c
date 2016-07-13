
#include <math.h>
#include <stdio.h>

#include "text.h"

// A---B
// | / |
// C---D
// | / |
// E---F

#define W 4
#define H 5

#define A 0,4
#define B 2,4
#define C 0,2
#define D 2,2
#define E 0,0
#define F 2,0
#define G 1,1
#define I 1,0
#define J 1,1
#define K 1,2
#define L 1,3


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
#define PL {5,J,L,K,C,D}
#define MN {2,D,C}
#define DT {2,G,I}

uint8_t digits[13][15] = {D0,D1,D2,D3,D4,D5,D6,D7,D8,D9,PL,MN,DT};
static char char_to_id(char c);

shapes_v
text_int(
    double n, 
    double x, 
    double y,
    double s,
    char   a) {

    shapes_v shapes;
    kv_init(shapes);
    char nc[10];
    sprintf(nc, "%d", (int)n);
    float k = (float)strlen(nc);
    int j = 0;
    for(char* c=nc; *c!='\0'; c++) {
        int d = char_to_id(*c);
        shape_v shape;
        kv_init(shape);        
        for(uint8_t i=0; i<digits[d][0];i++) {
            double o = a==0 ? W*j : a==1 ? W*(j-k) : W*(j-k/2.0); // char offset
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

static char
char_to_id(char c) {
    if(c>='0' && c<='9') return c-'0';
    else if(c=='+') return 10;
    else if(c=='-') return 11;
    else if(c=='.') return 12;
    return 12;
}