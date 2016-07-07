
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <shapefil.h>

#include "types.h"
#include "gui.h"
#include "wire.h"

static void on_error (int error, const char* description);
static void on_key   (GLFWwindow* window, int key, int scancode, int action, int mods);
static void on_mouse (GLFWwindow* window, double xpos, double ypos);
static void on_click (GLFWwindow* window, int button, int action, int mods);
static void on_scroll(GLFWwindow* window, double xoffset, double yoffset);
static void on_country(int cid);

static strings_v   dbf_get_column(const char* filename, const char* col);
static countries_v load_countries(const char *filename);

static countries_v countries;

int 
main(int argc, char** argv) {

    GLFWwindow* window;

    glfwSetErrorCallback(on_error);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(800, 600, "show mesh", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback        (window, on_key   );
    glfwSetScrollCallback     (window, on_scroll);
    glfwSetCursorPosCallback  (window, on_mouse );
    glfwSetMouseButtonCallback(window, on_click );

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    strings_v names = dbf_get_column("../../data/earth_110m.dbf", "name_long");
    countries = load_countries("../../data/earth_110m.shp");
    
    wire_init();
    gui_init(window, &names, on_country);

    while (!glfwWindowShouldClose(window)) {

        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        wire_draw(ratio, 0.0, 0.0, 0.01);

        glfwPollEvents();

        gui_logic();
        gui_draw();

        glfwSwapBuffers(window);
    }

    countries_destroy(&countries);
    strings_destroy(&names);

    gui_cleanup();
    wire_cleanup();
    
    glfwDestroyWindow(window);
    glfwTerminate();
    

    exit(EXIT_SUCCESS);
}


static void 
on_error(int error, const char* description) {
}

static void 
on_key(GLFWwindow* window, int key, int scancode, int action, int mods) {

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);

}

static void 
on_mouse(GLFWwindow* window, double xpos, double ypos){
}

static void 
on_click(GLFWwindow* window, int button, int action, int mods) {
}

static void 
on_scroll(GLFWwindow* window, double xoffset, double yoffset) {
}

static strings_v
dbf_get_column(const char* filename, const char* colname) {

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

static void on_country(int cid) {
    // for(uint32_t i=0; i<countries.n; i++) fprintf(stderr, "%d\n", i);
    wire_set_buffer(&countries.a[cid]);
}

static countries_v
load_countries(const char* filename) {

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
            kv_push(shape_v, shapes, shape);
        }
        SHPDestroyObject( shp );
        kv_push(shapes_v, countries, shapes);
    }
    SHPClose( hSHP );

end_loading:
    return countries;
}
