tnode_t*
tnode_new(triangle_t t) {
    tnode_t* node = malloc(sizeof(tnode_t));
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
    point_t min = (point_t) { HUGE_VAL, HUGE_VAL};
    point_t max = (point_t) {-HUGE_VAL,-HUGE_VAL};
    point_t o, s;

    for(uint32_t i=0; i<p.n; i++) {
        if (p.a[i].x>max.x) max.x = p.a[i].x;
        if (p.a[i].y>max.y) max.y = p.a[i].y;
        if (p.a[i].x<min.x) max.x = p.a[i].x;
        if (p.a[i].y<min.y) max.y = p.a[i].y;
    }
    o = (point_t) {(max.x+min.x)/2.0, (max.y+min.y)/2.0};
    s = (point_t) { max.x-min.x     ,  max.y-min.y     };
    double r = max(s.x, s.y);
    
    point_t t[3];
    triagnle_by_incircle(t, o, r);
}