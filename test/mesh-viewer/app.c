#include "main.h"

#define max(a,b) ((a)>(b)?(a):(b))

typedef struct {
    float x, y;
    float r, g, b;
} vertex_t;
typedef kvec_t(vertex_t) vertices_v;

static void load_mesh(char* filename);
static void load_shader(char* filename, char** buf);

GLuint vertex_buffers[BUFFERS];
vertices_v vertices[BUFFERS];

GLuint vertex_shader, fragment_shader, program;

char* vertex_shader_text;
char* fragment_shader_text;

GLint mvp_location, vpos_location, vcol_location;
mat4x4 m, p, mvp;

void set_buffer();

static void 
setup_shaders(){

    load_shader("shader.vert", &vertex_shader_text);
    load_shader("shader.frag", &fragment_shader_text);
    // SHADERS //
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
}

// initpoint
void 
app_init(int argc, char** argv) {

    posx = 0.0;
    posy = 0.0;
    scale = 1.0;

    setup_shaders();

    // MESH //
    if(argc>1){
        for(int i=0; i<BUFFERS; i++) {
            kv_init(vertices[i]);
        }
        load_mesh(argv[1]);
        for(int i=0; i<BUFFERS; i++) {
            printf("Buffer Data %d, %p\n",i, vertices[i].a);
        }
        glGenBuffers(BUFFERS, vertex_buffers);


        for(int i=0; i<BUFFERS; i++) {
            if(vertices[i].n == 0) continue;

            printf("Loaded %d: %zu points\n", i, vertices[i].n);
            printf("Bind %d buffer id: %d\n", i, vertex_buffers[i]);
            glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[i]);
            
            printf("Buffer Data %d, %p\n",i, vertices[i].a);
            glBufferData(GL_ARRAY_BUFFER, 
                vertices[i].n * sizeof(vertex_t), 
                vertices[i].a, GL_STATIC_DRAW);
            printf("Unbind %d\n", i);
            glBindBuffer(GL_ARRAY_BUFFER, 0);

        }
        current_buffer = 5;
    }
}

// draw loop
void 
app_draw(float ratio) {

    // SET BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffers[current_buffer]);        

    // SET ATTRIBUTES
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void*) 0);

    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                  sizeof(float) * 5, (void*) (sizeof(float) * 2));

    // SET VIEW
    mat4x4_identity(m);
    mat4x4_translate_in_place(m, posx, posy, 0.0);
    mat4x4_scale_aniso(m, m, scale,scale,scale);

    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);

    // SET SHADER
    glUseProgram(program);
    // send view matrix
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);

    // DRAW
    glDrawArrays(GL_TRIANGLES, 0, vertices[current_buffer].n);

    // UNSET SHADER
    glUseProgram(0);

    // UNSET ATTRIBUTES
    glDisableVertexAttribArray(vpos_location);
    glDisableVertexAttribArray(vcol_location);

    // UNSET BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

// cleanup at the end
void 
app_cleanup() {
    // cleanup
    fprintf(stderr, "Cleanup");
    glDeleteBuffers(BUFFERS, vertex_buffers);
    for(int i=0;i<10; i++) {
        kv_destroy(vertices[i]);
    }

    free(vertex_shader_text);
    free(fragment_shader_text);
}

void on_error(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void on_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    
    // fprintf(stderr, "key: %d scan: %d act: %d mods: %d\n", key, scancode, action, mods);
    // fprintf(stderr, "%d %d\n", posx, posy);

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (key == GLFW_KEY_LEFT  && (action == GLFW_PRESS || action == GLFW_REPEAT)) posx-=0.05;
    if (key == GLFW_KEY_RIGHT && (action == GLFW_PRESS || action == GLFW_REPEAT)) posx+=0.05;
    if (key == GLFW_KEY_UP    && (action == GLFW_PRESS || action == GLFW_REPEAT)) posy+=0.05;
    if (key == GLFW_KEY_DOWN  && (action == GLFW_PRESS || action == GLFW_REPEAT)) posy-=0.05;
    if (key == GLFW_KEY_MINUS && (action == GLFW_PRESS || action == GLFW_REPEAT)) scale-=0.2;
    if (key == GLFW_KEY_EQUAL && (action == GLFW_PRESS || action == GLFW_REPEAT)) scale+=0.2;

    if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9 && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        current_buffer = key - GLFW_KEY_0;
    }
}

void on_mouse(GLFWwindow* window, double xpos, double ypos){
}

void on_click(GLFWwindow* window, int button, int action, int mods) {
}

void on_scroll(GLFWwindow* window, double xoffset, double yoffset) {
    // printf("%f, %f\n", xoffset, yoffset);
    scale+=0.2*yoffset;
}

static void 
load_mesh(char* filename) {

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
        if(d<0 || d>9) continue;

        if(x>maxx) maxx=x; if(y>maxy) maxy=y;
        if(x<minx) minx=x; if(y<miny) miny=y;
        vertex_t v = (vertex_t) { x, y };
        kv_push(vertex_t, vertices[d], v);
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
    for(uint i=0; i<BUFFERS; i+=1){
        for(uint j=0; j<vertices[i].n; j++) {
            vertices[i].a[j].x = (vertices[i].a[j].x - cx)*scale; 
            vertices[i].a[j].y = (vertices[i].a[j].y - cy)*scale;
            vertices[i].a[j].r = ((j    )%256)/255.0;
            vertices[i].a[j].g = ((j+64 )%256)/255.0;
            vertices[i].a[j].b = ((j+128)%256)/255.0;
            // printf("%f %f %s",vertices->a[i].x, vertices->a[i].y, (i%8)?"":"\n");
        }
    }

    fclose(fp);
    if (line)
        free(line);
}

static void 
load_shader(
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
