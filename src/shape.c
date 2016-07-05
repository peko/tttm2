
#include <string.h>
#include <stdio.h>
#include <assert.h>

#include <shapefil.h>
#include <proj_api.h>

#include "shape.h"
#include "chanhull.h"

///////////
// SHAPE //
///////////

shape_t*
shape_new() {
   shape_t* shape = calloc(1, sizeof(shape_t));
   kv_init(shape->points);
   kv_init(shape->polys);
   kv_init(shape->hull);
   return shape;
}

shape_t*
shape_copy(const shape_t* source) {

    assert(source != NULL);

    shape_t* shape = shape_new();
    kv_copy(point_t, shape->points, source->points);
    kv_copy(point_t, shape->hull  , source->hull  );
    kv_copy(poly_t , shape->polys , source->polys );
    return shape;
}

void
shape_free(shape_t* shape) {

    assert( shape!=NULL );

    kv_destroy(shape->polys);
    kv_destroy(shape->points);
    kv_destroy(shape->hull);

    free(shape);
}

uint32_t
shape_add_point(
    shape_t* shape,
    point_t  point) {

    assert( shape!=NULL );

    kv_push(point_t, shape->points, point);
    return kv_size(shape->points)-1;
}

uint32_t
shape_add_poly(
    shape_t* shape,
    poly_t   poly) {

    assert( shape!=NULL );

    kv_push(poly_t, shape->polys, poly);
    return kv_size(shape->polys)-1;
}

// reproject shape
shape_t*
shape_proj(
    const shape_t* shape,
    const char*    from,
    const char*    to   ){

    shape_t* projected = shape_copy(shape);
    projPJ old_prj = pj_init_plus(from);
    projPJ new_prj = pj_init_plus(to);

    for(uint32_t i=0; i<kv_size(projected->points); i++) {
        point_t* p = &kv_A(projected->points, i);
        p->x *= DEG_TO_RAD;
        p->y *= DEG_TO_RAD;
        int32_t err = pj_transform(old_prj, new_prj, 1, 0, &p->x, &p->y, NULL);
        if (err)
            fprintf(stderr, "ERR%d %s\n", err, pj_strerrno(err));
        assert(err == 0);
    }

    for(uint32_t i=0; i<kv_size(projected->hull); i++) {
        point_t* p = &kv_A(projected->hull, i);
        p->x *= DEG_TO_RAD;
        p->y *= DEG_TO_RAD;
        int32_t err = pj_transform(old_prj, new_prj, 1, 0, &p->x, &p->y, NULL);
        if (err)
            fprintf(stderr, "ERR%d %s\n", err, pj_strerrno(err));
        assert(err == 0);
    }

    pj_free(old_prj);
    pj_free(new_prj);

    return projected;
}

// creat Chan's hull
void
shape_build_hull(shape_t* shape) {

    assert(shape!= NULL);

    points_v hull;
    kv_init(hull);
    kv_copy(point_t, hull, shape->points);
    uint32_t s = chanhull(hull.a, hull.n);
    uint32_t l = hull.n-s;

    // copy part of points vector
    kv_resize(point_t, shape->hull, l+1);
    shape->hull.n = l+1;
    // memcpy(shape->hull.a, hull.a+s, sizeof(point_t)*l);
    // shape->hull.a[l] = *(hull.a+s);
    // revert point direction
    for(uint32_t i=0; i<=l;i++) {
        shape->hull.a[l-i] = hull.a[s+i%l];
    }
    kv_destroy(hull);
}

// write shape
void
shape_write(const shape_t* shape, FILE* fp) {

    assert(shape != NULL && fp != NULL);

    point_t* p = shape->points.a;
    for(uint32_t i=0; i<shape->polys.n; i++) {
        poly_t* poly = &shape->polys.a[i];
        uint32_t s = poly->s;     // start point
        uint32_t e = s + poly->l; // end   point
        for(uint32_t j=s; j<e; j++) {
            fprintf(fp, "%f %f\n", p[j].x, p[j].y);
        }
        fprintf(fp, "\n");
    }
}

// write hull to file
void
shape_write_hull(const shape_t* shape, FILE* fp) {

    assert(shape != NULL && fp != NULL);

    point_t* p = shape->hull.a;
    for(uint32_t i=0; i<shape->hull.n; i++) {
        fprintf(fp, "%f %f\n", p[i].x, p[i].y);
    }
    fprintf(fp, "\n");
}

////////////
// SHAPES //
////////////

shapes_v*
shapes_new() {
    shapes_v* shapes = calloc(1,sizeof(shapes_v));
    kv_init(*shapes);
    return shapes;
}

void
shapes_free(shapes_v* shapes) {
    for(uint32_t i=0; i< kv_size(*shapes); i++) {
        shape_free(kv_A(*shapes, i));
    }
    kv_destroy(*shapes);
    free(shapes);
}

uint32_t
shapes_add_shape(
    shapes_v* shapes,
    shape_t*  shape) {

    assert( shape!=NULL && shapes!=NULL);

    kv_push(shape_t*, *shapes, shape);
    return kv_size(*shapes)-1;
}

void
shape_write_poly(const shape_t* shape, FILE* fp) {

    fprintf(fp, "# POINTS\n");
    fprintf(fp, "# <# of vertices> <dimension (must be 2)> <# of attributes> <# of boundary markers (0 or 1)>\n");

    uint32_t l = shape->points.n;
    // First line: <# of vertices> <dimension (must be 2)> <# of attributes> <# of boundary markers (0 or 1)>
    fprintf(fp, "%u 2 0 0\n", l);
    for(int i=0; i<l; i++) {
        // Following lines: <vertex #> <x> <y> [attributes] [boundary marker]
        point_t* p = shape->points.a+i;
        fprintf(fp, "%d %f %f\n", i, p->x, p->y);
    }

    fprintf(fp, "# POLYGONS\n");
    fprintf(fp, "# <# of segments> <# of boundary markers (0 or 1)>\n");

    // One line: <# of segments> <# of boundary markers (0 or 1)>
    fprintf(fp, "%u 0\n", l);
    uint32_t pid = 0;
    fprintf(fp, "# polygon %u\n", pid);
    poly_t p = shape->polys.a[pid];
    uint32_t e = p.s+p.l-1;
    for(int i=0; i<l; i++) {
        // Following lines: <segment #> <endpoint> <endpoint> [boundary marker]
        if(i == e) {
            fprintf(fp, "%d %d %d\n", i, i, p.s); // close part
            pid++;
            fprintf(fp, "# polygon %u\n", pid);
            if(pid < shape->polys.n) {
                p = shape->polys.a[pid];
                e = p.s+p.l-1;
            }
        } else {
            fprintf(fp, "%d %d %d\n", i, i, i+1);
        }
    }
    // One line: <# of holes>
    fprintf(fp, "0\n");
    // Following lines: <hole #> <x> <y>

    // Optional line: <# of regional attributes and/or area constraints>
    // Optional following lines: <region #> <x> <y> <attribute> <maximum area>
}

// LOAD SHP / DBF

shapes_v*
shapes_load(const char* name){

    char buf[256];

    strcpy(buf,name);
    strcat(buf,".dbf");
    shapes_load_dbf(buf, "name_long");

    strcpy(buf, name);
    strcat(buf, ".shp");
    shapes_v* shapes = shapes_load_shp(buf);

    return shapes;
}

// extract info from dbf
void
shapes_load_dbf(const char* filename, const char* column){

    DBFHandle hDBF;

    hDBF = DBFOpen( filename, "rb" );
    if( hDBF == NULL ) {
        fprintf(stderr, "DBFOpen(%s,\"r\") failed.\n", filename );
        return;
    }

    // INFO ABOUT DBF
    // int  *panWidth;
    // int  nWidth, nDecimals;
    // fprintf (stderr, "Info for %s\n", filename);
    // i = DBFGetFieldCount(hDBF);
    // fprintf (stderr, "%d Columns,  %d Records in file\n",i,DBFGetRecordCount(hDBF));
    // panWidth = (int *) malloc( DBFGetFieldCount( hDBF ) * sizeof(int) );
    // for( int i = 0; i < DBFGetFieldCount(hDBF); i++ ) {
    //     DBFFieldType    eType;
    //     char szTitle[256];
    //     eType = DBFGetFieldInfo( hDBF, i, szTitle, &nWidth, &nDecimals );
    //     fprintf(stderr, "%4d: %10s %c", i, szTitle, i%4 ? '|':'\n');
    // }
    // fprintf(stderr, "\n");

    // print names
    uint32_t fid = DBFGetFieldIndex(hDBF, column);
    for(uint32_t i = 0; i < DBFGetRecordCount(hDBF); i++ ) {
       char* name_long = (char *) DBFReadStringAttribute(hDBF, i, fid);
       fprintf(stderr, "%d: %s\n", i, name_long);
    }

    DBFClose( hDBF );
}


// LOAD SHP

shapes_v*
shapes_load_shp(const char* filename) {

    shapes_v* shapes = shapes_new();

    double  adfMinBound[4],
            adfMaxBound[4];

    // Read file
    SHPHandle hSHP = SHPOpen( filename, "rb" );
    assert(hSHP != NULL);

    // Print shape bounds
    int shapes_count, shapes_vype;
    SHPGetInfo( hSHP, &shapes_count, &shapes_vype, adfMinBound, adfMaxBound );

    // printf( "Shapefile Type: %s   # of Shapes: %d\n\n",
    //         SHPTypeName( shapes_vype ), shapes_count );

    // printf( "File Bounds: (%.15g,%.15g,%.15g,%.15g)\n"
    //         "         to  (%.15g,%.15g,%.15g,%.15g)\n",
    //         adfMinBound[0], adfMinBound[1],
    //         adfMinBound[2], adfMinBound[3],
    //         adfMaxBound[0], adfMaxBound[1],
    //         adfMaxBound[2], adfMaxBound[3]);

    // Iterate through shapes
    for(int i = 0; i < shapes_count; i++ ) {
        shape_t* shape = shape_new();
        SHPObject *shp = SHPReadObject(hSHP, i);

        assert(shp != NULL);

        if(shp->nParts == 0) continue;
        // first part starts at point 0
        assert(shp->panPartStart[0] == 0);

        // add points
        for(uint32_t j=0; j< shp->nVertices; j++) {
            point_t p = (point_t){shp->padfX[j], shp->padfY[j]};
            shape_add_point(shape, p);
        }
        uint32_t parts = shp->nParts;
        for (uint32_t j=0; j<parts; j++) {
            uint32_t s = shp->panPartStart[j];
            uint32_t e = (j+1 < parts) ?
                shp->panPartStart[j+1]:
                shp->nVertices;
            poly_t p = (poly_t){s, e-s};
            shape_add_poly(shape, p);
        }
        shape->bbox = (bbox_t) {
            (point_t){shp->dfXMin, shp->dfYMin},
            (point_t){shp->dfXMax, shp->dfYMax}
        };
        shapes_add_shape(shapes, shape);
        SHPDestroyObject( shp );
    }
    SHPClose( hSHP );

    return shapes;
}

shapes_v*
shapes_proj(const shapes_v* shapes,
            const char* from,
            const char* to) {

    shapes_v* projected = shapes_new();
    for(uint32_t i=0; i<kv_size(*shapes); i++){
        shape_t* p = shape_proj(kv_A(*shapes, i), from, to);
        shapes_add_shape(projected, p);
    }
    return projected;
}
