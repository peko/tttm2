#include <stdio.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include "linmath.h"

#include "wire.h"

typedef struct {
    float x, y;
    float r, g, b;
} vertex_t;
typedef kvec_t(vertex_t) vertices_v;

static GLuint vertex_shader, fragment_shader, program;
static GLuint vertex_buffer = 0;
static GLuint vertex_count  = 0;
static GLuint shapes_count  = 0;
static GLuint shape_lenghts[256] = {0};

static char* vertex_shader_text;
static char* fragment_shader_text;

static GLint mvp_location, vpos_location, vcol_location;

static void setup_shaders();
static void load_shader(char* filename, char** buf);


void 
wire_init() {
    setup_shaders();
}

void
wire_set_buffer(shapes_v* shapes) {
    
    if(vertex_buffer>0) glDeleteBuffers(1, &vertex_buffer);

    // sum shpaes points
    vertex_count = 0;
    shapes_count = shapes->n;
    for(uint32_t s=0; s<shapes->n; s++) {
        vertex_count += shapes->a[s].n;
        shape_lenghts[s] = shapes->a[s].n;
    }

    // fill buffer
    vertex_t vertices[vertex_count];
    uint32_t i=0;
    for(uint32_t s=0; s<shapes->n; s++) {
        for(uint32_t p=0; p<shapes->a[s].n; p++) {
            point_t* pnt = &shapes->a[s].a[p];
            vertices[i++] = (vertex_t) {pnt->x, pnt->y, 1.0, 1.0, 1.0};
        }
    }

    // send buffer to vram
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);    
    glBufferData(GL_ARRAY_BUFFER, 
        vertex_count * sizeof(vertex_t), 
        vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// draw loop
void 
wire_draw(float ratio, float x, float y, float scale) {

    mat4x4 m, p, mvp;

    // SET BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);        

    // SET ATTRIBUTES
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void*) 0);

    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                  sizeof(float) * 5, (void*) (sizeof(float) * 2));

    // SET VIEW
    mat4x4_identity(m);
    mat4x4_translate_in_place(m, x, y, 0.0);
    mat4x4_scale_aniso(m, m, scale, scale, scale);

    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);

    // SET SHADER
    glUseProgram(program);
    // send view matrix
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);

    // DRAW
    uint32_t start = 0;
    for(uint32_t i=0; i<shapes_count; i++) {
        glDrawArrays(GL_LINE_LOOP, start, shape_lenghts[i]);
        start+=shape_lenghts[i];
    }
    // glDrawArrays(GL_LINE_LOOP, 0, vertex_count);

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
wire_cleanup() {
    // cleanup
    if(vertex_buffer>0) glDeleteBuffers(1, &vertex_buffer);

    free(vertex_shader_text);
    free(fragment_shader_text);
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

static void 
setup_shaders(){

    load_shader("wire_shader.vert", &vertex_shader_text);
    load_shader("wire_shader.frag", &fragment_shader_text);
    
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
