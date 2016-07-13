#include <stdint.h>
#include <string.h>

#include <shapefil.h>
#include <proj_api.h>

#include "shape.h"

countries_v
shape_load_countries(const char* filename) {

    countries_v countries;
    kv_init(countries);

    double  adfMinBound[4],
            adfMaxBound[4];

    // Read file
    SHPHandle hSHP = SHPOpen( filename, "rb" );
    if(hSHP == NULL) goto end_loading;

    // Print shape bounds
    int country_count, shapes_vype;
    SHPGetInfo( hSHP, &country_count, &shapes_vype, adfMinBound, adfMaxBound );
    
    fprintf(stderr, "Load %d countries\n", country_count);
    // Iterate through countries
    for(int i = 0; i < country_count; i++ ) {
                        
        SHPObject *shp = SHPReadObject(hSHP, i);
        if(shp == NULL) goto end_loading;

        if(shp->nParts == 0) continue;

        // first part starts at point 0
        if(shp->panPartStart[0] != 0) goto end_loading;

        // collect parts of country
        shapes_v shapes;
        kv_init(shapes);
        shapes.min = (point_t){shp->dfXMin, shp->dfYMin};
        shapes.max = (point_t){shp->dfXMax, shp->dfYMax};

        uint32_t parts = shp->nParts;
        double k = 0.0;
        for (uint32_t j=0; j<parts; j++) {
            // start index
            uint32_t s = shp->panPartStart[j];
            // end index - start of next minus one, or end
            uint32_t e = (j+1 < parts) ?
                shp->panPartStart[j+1]:
                shp->nVertices;
            shape_v shape;
            kv_init(shape);
            // collect points of part
            for(uint32_t i=s; i<e; i++){
                point_t p = (point_t){shp->padfX[i], shp->padfY[i]};
                kv_push(point_t, shape, p);
                // cumulitive average for center
                if(k>=1.0) {
                    shapes.center.x = (k-1.0)/k*shapes.center.x + p.x/k;
                    shapes.center.y = (k-1.0)/k*shapes.center.y + p.y/k;
                }else {
                    shapes.center.x = p.x;
                    shapes.center.y = p.y;
                }
                k+=1.0;
            }
            kv_push(shape_v, shapes, shape);
        }
        SHPDestroyObject( shp );
        kv_push(shapes_v, countries, shapes);
    }
    SHPClose( hSHP );

end_loading:
    return countries;
}

shapes_v
shape_load_globe(const char* filename) {

    shapes_v globe;
    kv_init(globe);

    double  adfMinBound[4],
            adfMaxBound[4];

    // Read file
    SHPHandle hSHP = SHPOpen( filename, "rb" );
    if(hSHP == NULL) goto end_loading;

    // Print shape bounds
    int country_count, shapes_vype;
    SHPGetInfo( hSHP, &country_count, &shapes_vype, adfMinBound, adfMaxBound );
    
    fprintf(stderr, "Load %d countries\n", country_count);
    // Iterate through countries
    for(int i = 0; i < country_count; i++ ) {
                        
        SHPObject *shp = SHPReadObject(hSHP, i);
        if(shp == NULL) goto end_loading;

        if(shp->nParts == 0) continue;

        // first part starts at point 0
        if(shp->panPartStart[0] != 0) goto end_loading;

        // collect parts of country
        uint32_t parts = shp->nParts;
        for (uint32_t j=0; j<parts; j++) {
            // start index
            uint32_t s = shp->panPartStart[j];
            // end index - start of next minus one, or end
            uint32_t e = (j+1 < parts) ?
                shp->panPartStart[j+1]:
                shp->nVertices;
            shape_v shape;
            kv_init(shape);
            // collect points of part
            for(uint32_t i=s; i<e; i++){
                point_t p = (point_t){shp->padfX[i], shp->padfY[i]};
                kv_push(point_t, shape, p);
            }
            kv_push(shape_v, globe, shape);
        }
        SHPDestroyObject( shp );
    }
    SHPClose( hSHP );

end_loading:
    return globe;
}


// reproject shapes
shapes_v
shape_proj(
    const shapes_v* shapes, 
    const char* from, 
    const char* to){

    projPJ old_prj = pj_init_plus(from);
    projPJ new_prj = pj_init_plus(to);

    shapes_v shapes_prj;
    kv_init(shapes_prj);
    
    shapes_prj.min = (point_t){ INFINITY, INFINITY};
    shapes_prj.max = (point_t){-INFINITY,-INFINITY};
    double k = 0.0;
    for(uint32_t s=0; s<shapes->n; s++) {
        shape_v* shape = &shapes->a[s];
        shape_v shape_prj;
        kv_init(shape_prj);
        for(uint32_t p=0; p<shape->n; p++) {
            point_t pnt = shape->a[p];
            pnt.x *= DEG_TO_RAD;
            pnt.y *= DEG_TO_RAD;
            int32_t err = pj_transform(old_prj, new_prj, 1, 0, &pnt.x, &pnt.y, NULL);
            if (err) {
                fprintf(stderr, "ERR%d %s\n", err, pj_strerrno(err));
                continue;
            }

            // cumulitive average for center
            if(k>=1.0) {
                shapes_prj.center.x = (k-1.0)/k*shapes_prj.center.x + pnt.x/k;
                shapes_prj.center.y = (k-1.0)/k*shapes_prj.center.y + pnt.y/k;
            }else {
                shapes_prj.center.x = pnt.x;
                shapes_prj.center.y = pnt.y;
            }
            k+=1.0;

            // new bounds
            if(pnt.x>shapes_prj.max.x) shapes_prj.max.x = pnt.x;
            if(pnt.y>shapes_prj.max.y) shapes_prj.max.y = pnt.y;
            if(pnt.x<shapes_prj.min.x) shapes_prj.min.x = pnt.x;
            if(pnt.y<shapes_prj.min.y) shapes_prj.min.y = pnt.y;

            kv_push(point_t, shape_prj, pnt);
        }
        kv_push(shape_v, shapes_prj, shape_prj);
    }

    pj_free(old_prj);
    pj_free(new_prj);

    return shapes_prj;
}

strings_v
shape_load_names(const char* filename, const char* colname) {

    DBFHandle hDBF;
    strings_v col;
    kv_init(col);

    hDBF = DBFOpen( filename, "rb" );
    if( hDBF == NULL ) {
        fprintf(stderr, "DBFOpen(%s,\"r\") failed.\n", filename );
        return col;
    }
    uint32_t fid = DBFGetFieldIndex(hDBF, colname);
    for(uint32_t i = 0; i < DBFGetRecordCount(hDBF); i++ ) {
        char* str = (char *) DBFReadStringAttribute(hDBF, i, fid);
        if(str != NULL)
            kv_push(char*, col, strdup(str));
        else            
            kv_push(char*, col, NULL);
    }

    DBFClose( hDBF );
    return col;
}

