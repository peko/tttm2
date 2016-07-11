
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "types.h"
#include "linmath.h"
#include "shape.h"
#include "grid.h"
#include "gui.h"
#include "wire.h"
#include "vbo.h"

static void on_error  (int error, const char* description);
static void on_key    (GLFWwindow* window, int key, int scancode, int action, int mods);
static void on_mouse  (GLFWwindow* window, double xpos, double ypos);
static void on_click  (GLFWwindow* window, int button, int action, int mods);
static void on_scroll (GLFWwindow* window, double xoffset, double yoffset);
static void on_country(int cid);

static void next_country();
static void prev_country();
static void set_country(int cid);

static countries_v countries;
static strings_v names;
static vbo_t grid_vbo    = (vbo_t){0};
static vbo_t country_vbo = (vbo_t){0};

static int current_country = 0;

static double scale = 1.0;
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

    names = shape_load_names("../../data/10m.dbf", "name_long");
    countries = shape_load_countries("../../data/10m.shp");

    wire_init();
    gui_init(window, &names, on_country);

    while (!glfwWindowShouldClose(window)) {

        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        if(grid_vbo.id != 0)
            wire_draw(&grid_vbo,(vec3){0.25,0.25,0.25}, ratio, 0.0, 0.0, scale, false);

        if(country_vbo.id != 0)
            wire_draw(&country_vbo,(vec3){1.0,1.0,1.0}, ratio, 0.0, 0.0, scale, true);

        glfwPollEvents();

        gui_logic(width, height);
        gui_draw();

        glfwSwapBuffers(window);
    }

    if(country_vbo.id != 0) vbo_destroy(&country_vbo);
    if(grid_vbo.id    != 0) vbo_destroy(&grid_vbo);

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
    if (key == 46 && action == GLFW_PRESS)
        next_country();
    if (key == 44 && action == GLFW_PRESS)
        prev_country();
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

static void 
on_country(int cid) {
    
    current_country = cid;
    set_country(cid);
    printf("%d: %s\n",current_country, names.a[current_country]);
}

//////////////////////////////////////////////////////////////

static void
next_country(){
    current_country++;
    if (current_country > countries.n) current_country = 0;
    on_country(current_country);
}

static void
prev_country(){
    current_country--;
    if (current_country < 0) current_country = countries.n-1;
    on_country(current_country);
}

void
set_country(int cid) {

    static char* from = "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";
    static char  to[256];
    
    shapes_v* country = &countries.a[cid];
    sprintf(to, "+proj=laea +lat_0=%f +lon_0=%f +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs", 
        country->center.y, country->center.x);
    
    shapes_v country_prj = shape_proj(country, from, to);
    double w = (country_prj.max.x-country_prj.min.x);
    double h = (country_prj.max.y-country_prj.min.y);
    scale = 1.0/(w>h?w:h);

    if(country_vbo.id != 0) vbo_destroy(&country_vbo);
    country_vbo = vbo_new(&country_prj);
    shapes_destroy(&country_prj);

    //GRID
    shapes_v grid = grid_make(&country->min, &country->max);
    shapes_v grid_prj = shape_proj(&grid, from, to);

    if(grid_vbo.id != 0) vbo_destroy(&grid_vbo);
    grid_vbo = vbo_new(&grid_prj);

    shapes_destroy(&grid);
    shapes_destroy(&grid_prj);
}