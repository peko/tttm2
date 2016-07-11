#include <stdio.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "wire.h"

static GLuint vertex_shader, fragment_shader, program;
static char* vertex_shader_text;
static char* fragment_shader_text;

static GLint mvp_location, vpos_location, col_location;

static void setup_shaders();
static void load_shader(char* filename, char** buf);

void 
wire_init() {
    setup_shaders();
}

// draw loop
void 
wire_draw(vbo_t* vbo, vec3 col, float ratio, float x, float y, float scale) {

    mat4x4 m, p, mvp;

    // SET BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, vbo->id);        

    // SET ATTRIBUTES
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(vertex_t), (void*) 0);

    //glEnableVertexAttribArray(vcol_location);
    //glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
    //              sizeof(float) * 5, (void*) (sizeof(float) * 2));

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
    glUniform3fv(col_location,  1, (const GLfloat*) col);

    // DRAW
    uint32_t start = 0;
    for(uint32_t i=0; i<vbo->shapes; i++) {
        GLuint l = vbo->parts[i];
        glDrawArrays(GL_LINE_LOOP, start, l);
        start+= l;
    }
    // glDrawArrays(GL_LINE_LOOP, 0, vertex_count);

    // UNSET SHADER
    glUseProgram(0);

    // UNSET ATTRIBUTES
    glDisableVertexAttribArray(vpos_location);

    // UNSET BUFFER
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// cleanup at the end
void 
wire_cleanup() {
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
    col_location  = glGetUniformLocation(program, "col");
    
    vpos_location = glGetAttribLocation(program, "vPos");
}
