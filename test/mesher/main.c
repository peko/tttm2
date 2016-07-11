
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "types.h"
#include "linmath.h"
#include "shape.h"
#include "gui.h"
#include "wire.h"
#include "vbo.h"

static void on_error  (int error, const char* description);
static void on_key    (GLFWwindow* window, int key, int scancode, int action, int mods);
static void on_mouse  (GLFWwindow* window, double xpos, double ypos);
static void on_click  (GLFWwindow* window, int button, int action, int mods);
static void on_scroll (GLFWwindow* window, double xoffset, double yoffset);
static void on_country(int cid);

static countries_v countries;
static vbo_t country_vbo = (vbo_t){0};

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

    strings_v names = shape_load_names("../../data/110m.dbf", "name_long");
    countries       = shape_load_countries("../../data/110m.shp");

    wire_init();
    gui_init(window, &names, on_country);

    while (!glfwWindowShouldClose(window)) {

        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        if(country_vbo.id != 0)
            wire_draw(&country_vbo,(vec3){1.0,1.0,1.0}, ratio, 0.0, 0.0, scale);

        glfwPollEvents();

        gui_logic(width, height);
        gui_draw();

        glfwSwapBuffers(window);
    }

    if(country_vbo.id != 0) vbo_destroy(&country_vbo);

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

static void on_country(int cid) {
    
    static char* from = "+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs";
    static char  to[256];
    
    // clear old vbo
    if(country_vbo.id != 0) vbo_destroy(&country_vbo);
    
    shapes_v* country = &countries.a[cid];
    // projections
    sprintf(to, "+proj=laea +lat_0=%f +lon_0=%f +x_0=0 +y_0=0 +ellps=WGS84 +datum=WGS84 +units=m +no_defs", 
        country->center.y, country->center.x);
 
    shapes_v projected = shape_proj(country, from, to);
    double w = (projected.max.x-projected.min.x);
    double h = (projected.max.y-projected.min.y);
    scale = 1.0/(w>h?w:h);
    country_vbo = vbo_new(&projected);
    shapes_destroy(&projected);
}

