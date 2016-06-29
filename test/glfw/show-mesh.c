
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "kvec.h"

#include <stdlib.h>
#include <stdio.h>

typedef struct {
    float x, y;
    float r, g, b;
} vertex_t;
typedef kvec_t(vertex_t) vertices_v;


static const struct
{
    float x, y;
    float r, g, b;
} test[3] =
{
    { -0.6f, -0.4f, 1.f, 0.f, 0.f },
    {  0.6f, -0.4f, 0.f, 1.f, 0.f },
    {   0.f,  0.6f, 0.f, 0.f, 1.f }
};

#define max(a,b) ((a)>(b)?(a):(b))

static void 
load_file(
    char*  filename, 
    char** buf) {

    long length;
    FILE* fp = fopen (filename, "rb");
    
    if(fp) {
      fseek (fp, 0, SEEK_END);
      length = ftell (fp);
      fseek (fp, 0, SEEK_SET);
      *buf = malloc(length);
      if (*buf) {
        fread (*buf, 1, length, fp);
      }
      fclose (fp);
    }
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
        if(d!=6) continue;

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

static void error_callback(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

static double mousex, mousey;
static void cursor_pos_callback(GLFWwindow* window, double xpos, double ypos){
    mousex = xpos;
    mousey = ypos;
}
static void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    // if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
    //     popup_menu();
    // int state = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    // if (state == GLFW_PRESS)
    //     upgrade_cow();
}

static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {

}

int main(int argc, char** argv) {

    GLFWwindow* window;
    GLuint vertex_buffer, vertex_shader, fragment_shader, program;
    GLint mvp_location, vpos_location, vcol_location;

    char* vertex_shader_text;
    char* fragment_shader_text;

    load_file("shader.vert", &vertex_shader_text);
    load_file("shader.frag", &fragment_shader_text);

    glfwSetErrorCallback(error_callback);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

    window = glfwCreateWindow(800, 600, "show mesh", NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, cursor_pos_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    // MESH //
    vertices_v vertices;
    kv_init(vertices);

    if(argc>1){
        glGenBuffers(1, &vertex_buffer);
        glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
        load_mesh(argv[1], &vertices);
        printf("Loaded %zu points\n", vertices.n);
        glBufferData(GL_ARRAY_BUFFER, vertices.n * sizeof(vertex_t), vertices.a, GL_STATIC_DRAW);
        // glBufferData(GL_ARRAY_BUFFER, sizeof(test), &test, GL_STATIC_DRAW);
        printf("SIZEOF test array %d vertex_t %d\n", sizeof(test), sizeof(vertex_t));
    }

    // SHADERS //
    {
        vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, (const char**)&vertex_shader_text, NULL);
        glCompileShader(vertex_shader);

        fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, (const char**)&fragment_shader_text, NULL);
        glCompileShader(fragment_shader);

        program = glCreateProgram();
        glAttachShader(program, vertex_shader);
        glAttachShader(program, fragment_shader);
        glLinkProgram(program);

        mvp_location  = glGetUniformLocation(program, "MVP");
        vpos_location = glGetAttribLocation(program, "vPos");
        vcol_location = glGetAttribLocation(program, "vCol");

        glEnableVertexAttribArray(vpos_location);
        glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                              sizeof(float) * 5, (void*) 0);

        glEnableVertexAttribArray(vcol_location);
        glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                              sizeof(float) * 5, (void*) (sizeof(float) * 2));
    }
    
    while (!glfwWindowShouldClose(window)) {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        mat4x4_identity(m);
        // mat4x4_rotate_Z(m, m, mousey/1000.0);
        // mat4x4_rotate_Y(m, m, mousex/1000.0);
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);
        glDrawArrays(GL_TRIANGLES, 0, vertices.n);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    kv_destroy(vertices);
    free(vertex_shader_text);
    free(fragment_shader_text);

    exit(EXIT_SUCCESS);
}

