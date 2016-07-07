#include <stdint.h>

#include "types.h"

void 
strings_destroy(strings_v* str) {
    // clear strings
    for(uint32_t i=0; i< str->n; i++) free(str->a[i]);
    // clear vector
    kv_destroy(*str);
}

void
shape_destroy(shape_v* s) {
    kv_destroy(*s);
}

void
shapes_destroy(shapes_v* s) {
    for(uint32_t i=0; i<s->n; i++) 
        shape_destroy(&s->a[i]);
    kv_destroy(*s);
}

void
countries_destroy(countries_v* c) {
    for(uint32_t i=0; i<c->n; i++)
        shapes_destroy(&c->a[i]);
    kv_destroy(*c);
}
