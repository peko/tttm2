#include <stdio.h>
#include <assert.h>

#include "shape.h"
#include "mesh.h"
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
    
    // mesh_t* mesh = mesh_from_shape(shape);
    // fprintf(stderr, "Write ttree-mesh.txt\n");
    // fp = fopen("ttree-mesh.txt", "w");
    // mesh_write(mesh, fp);
    // fclose(fp);
    
    mesh_t* mesh2 = mesh_new();

    point_t a,b,c;
    a = (point_t){ 0,1};
    b = (point_t){ 1,0};
    c = (point_t){-1,0};
    kv_push(point_t, mesh2->points, a);
    kv_push(point_t, mesh2->points, b);
    kv_push(point_t, mesh2->points, c);
    triangle_t t = (triangle_t){0,1,2};
    kv_push(triangle_t, mesh2->triangles, t);
    
    fprintf(stderr, "Write ttree-mesh.txt\n");
    fp = fopen("ttree-mesh.txt", "w");
    mesh_write(mesh2, fp);
    fclose(fp);
    
    ttree_t* tree = ttree_from_mesh(&mesh2->points, &mesh2->triangles, 5);
    if(tree != NULL) {
        fprintf(stderr, "Write ttree-ttree.txt\n");
        fp = fopen("ttree-ttree.txt", "w");
        ttree_write(tree, fp);
        fclose(fp);
        ttree_free(tree);
    }
    mesh_free(mesh2); 

    // mesh_free(mesh);
    shapes_free(shapes_pr);
    shapes_free(shapes);

    return 0;
}