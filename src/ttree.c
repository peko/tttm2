#include <stdio.h>
#include <math.h>

#include "ttree.h"

tnode_t*
tnode_new(triangle_t t) {
    tnode_t* node = calloc(1, sizeof(tnode_t));
    node->triangle = t;
    return node;
}

void 
tnode_free(tnode_t* node) {
    
    if(node == NULL) return;

    if(node->children[0] != NULL) tnode_free(node->children[0]);
    if(node->children[1] != NULL) tnode_free(node->children[1]);
    if(node->children[2] != NULL) tnode_free(node->children[2]);
    if(node->children[3] != NULL) tnode_free(node->children[3]);

    free(node);
}

ttree_t*
ttree_new(point_t p[3]) {

    ttree_t* tree = malloc(sizeof(ttree_t));

    kv_init(tree->points);

    kv_push(point_t, tree->points, p[0]);
    kv_push(point_t, tree->points, p[1]);
    kv_push(point_t, tree->points, p[2]);

    tree->root = tnode_new((triangle_t){0,1,2});

    return tree;
}


void
ttree_free(ttree_t* tree) {
    
    if(tree == NULL) return;

    tnode_free(tree->root);
    kv_destroy(tree->points);
    free(tree);
}

ttree_t*
ttree_from_points(const points_v p) {

    point_t o, s;
        
    { // max/min shadowing
        point_t min = (point_t) { HUGE_VAL, HUGE_VAL};
        point_t max = (point_t) {-HUGE_VAL,-HUGE_VAL};

        for(uint32_t i=0; i<p.n; i++) {
            if (p.a[i].x>max.x) max.x = p.a[i].x;
            if (p.a[i].y>max.y) max.y = p.a[i].y;
            if (p.a[i].x<min.x) min.x = p.a[i].x;
            if (p.a[i].y<min.y) min.y = p.a[i].y;
        }
        o = (point_t) {(max.x+min.x)/2.0, (max.y+min.y)/2.0};
        s = (point_t) { max.x-min.x     ,  max.y-min.y     };
    }
    double r = s.x>s.y ? s.x : s.y;
    
    point_t t[3];
    triangle_by_incircle(t, o, r);

    ttree_t* ttree = ttree_new(t);

    for(uint32_t i=0; i<p.n; i++) {
        ttree_split_node(ttree, ttree->root, &p.a[i]);
    }

    return ttree;
}

void 
ttree_split_node(
    ttree_t* tt, 
    tnode_t* tn, 
    point_t* p){

    point_t a,b,c;

    a = tt->points.a[tn->triangle.a];
    b = tt->points.a[tn->triangle.b];
    c = tt->points.a[tn->triangle.c];

    double u,v,w;
    {
        point_t bc = point_to_barycentric(p, 
            &a, 
            &tt->points.a[tn->triangle.b], 
            &tt->points.a[tn->triangle.c]);

        // check if point outside
        u = bc.x;
        v = bc.y;
        w = 1-bc.x - bc.y;
    }
    if (u < 0 || v < 0 || w<0) return;

    uint32_t cid = 0;
    if(u>0.5) cid = 1;
    else if(v>0.5) cid = 2;
    else if(w>0.5) cid = 3;

    point_t ab, bc, ca;
    ab = point_mid(a,b);
    bc = point_mid(b,c);
    ca = point_mid(c,a);

    tnode_t** child = &tn->children[cid];
    if (*child == NULL) {
        if(cid == 0)
            *child = tnode_new(triangle_new(&tt->points, &ab, &bc, &ca));
        else if(cid == 1)
            *child = tnode_new(triangle_new(&tt->points, &a, &ab, &ca));
        else if(cid == 2)
            *child = tnode_new(triangle_new(&tt->points, &b, &bc, &ab));
        else if(cid == 3)
            *child = tnode_new(triangle_new(&tt->points, &c, &ca, &bc));
    } else {
        ttree_split_node(tt, *child, p);        
    }
}

void
ttree_write(
    ttree_t* tree, 
    FILE*    fp){
    tnode_write(tree->root, tree, 0, fp);
}

static void
triangle_write(
    triangle_t* t,
    points_v*   p, 
    uint32_t    d,
    FILE*       fp){
    
    point_t a,b,c;
    a = p->a[t->a];
    b = p->a[t->b];
    c = p->a[t->c];

    fprintf(fp, "%f %f %d\n", a.x, a.y, d); 
    fprintf(fp, "%f %f %d\n", b.x, b.y, d); 
    fprintf(fp, "%f %f %d\n", c.x, c.y, d); 
    fprintf(fp, "%f %f %d\n", a.x, a.y, d);
    fprintf(fp, "\n"); 
} 

void
tnode_write(
    tnode_t* tn,
    ttree_t* tt,
    uint32_t d,
    FILE*    fp){

    // if (tn->children[0] == NULL&&
        // tn->children[1] == NULL&&
        // tn->children[2] == NULL&&
        // tn->children[3] == NULL)
    triangle_write(&tn->triangle, &tt->points, d, fp);

    if(tn->children[0]) tnode_write(tn->children[0], tt, d+1, fp);
    if(tn->children[1]) tnode_write(tn->children[1], tt, d+1, fp);
    if(tn->children[2]) tnode_write(tn->children[2], tt, d+1, fp);
    if(tn->children[3]) tnode_write(tn->children[3], tt, d+1, fp);
}