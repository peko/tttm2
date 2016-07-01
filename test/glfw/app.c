#include "main.h"


#define max(a,b) ((a)>(b)?(a):(b))

typedef struct {
    float x, y;
    float r, g, b;
} vertex_t;
typedef kvec_t(vertex_t) vertices_v;

static void 
load_mesh(
    char* filename, 
    vertices_v* vertices);

GLuint vertex_buffer;
vertices_v vertices;
// initpoint
void init(int argc, char** argv) {
    // MESH //
    if(argc>1){
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        kv_init(vertices);
        load_mesh(argv[1], &vertices);
        printf("Loaded %zu points\n", vertices.n);
        glBufferData(GL_ARRAY_BUFFER, vertices.n * sizeof(vertex_t), vertices.a, GL_STATIC_DRAW);
    }
}

// draw loop
void draw() {
    glDrawArrays(GL_TRIANGLES, 0, vertices.n);
}

// cleanup at the end
void cleanup() {
    kv_destroy(vertices);
}

void on_error(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void on_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
    // fprintf(stderr, "%d = %d %d\n", key, GLFW_KEY_UP, action);
    // fprintf(stderr, "%d %d\n", posx, posy);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_LEFT  && (action == GLFW_PRESS || action == GLFW_REPEAT)) posx-=0.05;
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) posx+=0.05;
    if (key == GLFW_KEY_UP    && (action == GLFW_PRESS || action == GLFW_REPEAT)) posy+=0.05;
    if (key == GLFW_KEY_DOWN  && (action == GLFW_PRESS || action == GLFW_REPEAT)) posy-=0.05;

}

void on_mouse(GLFWwindow* window, double xpos, double ypos){
}

void on_click(GLFWwindow* window, int button, int action, int mods) {
}

void on_scroll(GLFWwindow* window, double xoffset, double yoffset) {
}

static void 
load_mesh(
    char* filename, 
    vertices_v* vertices) {

    FILE* fp;
    char* line = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen(filename, "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    float minx, miny, maxx, maxy, cx, cy, w, h;
    minx = HUGE_VAL; miny = HUGE_VAL;
    maxx =-HUGE_VAL; maxy =-HUGE_VAL;
    while ((read = getline(&line, &len, fp)) != -1) {
        if(line[0]=='\n') continue;
        char* e;
        double x = strtod(line, &e); 
        double y = strtod(   e, &e);
        int    d = strtol(   e, &e, 10);
        if(d!=8) continue;

        if(x>maxx) maxx=x; if(y>maxy) maxy=y;
        if(x<minx) minx=x; if(y<miny) miny=y;
        vertex_t v = (vertex_t) { x, y, 1.0, 0.5, 0.5};
        kv_push(vertex_t, *vertices, v);
    }

    w = maxx-minx;
    h = maxy-miny;
    cx = minx + w/2.0;
    cy = miny + h/2.0;

    printf("min %f %f\n", minx, miny);
    printf("max %f %f\n", maxx, maxy);
    printf("siz %f %f\n", w, h);
    printf("cnt %f %f\n", cx, cy);

    // FIT model to 1x1 box
    double scale = 2.0/max(w,h);
    printf("scale %f\n", scale);
    for(uint i=0; i<vertices->n; i++) {
        vertices->a[i].x = (vertices->a[i].x - cx)*scale; 
        vertices->a[i].y = (vertices->a[i].y - cy)*scale;
        vertices->a[i].r = ((i    )%256)/255.0;
        vertices->a[i].g = ((i+64 )%256)/255.0;
        vertices->a[i].b = ((i+128)%256)/255.0;
        // printf("%f %f %s",vertices->a[i].x, vertices->a[i].y, (i%8)?"":"\n");
    }

    fclose(fp);
    if (line)
        free(line);
}
