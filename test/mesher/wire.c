#include "wire.h"

#define max(a,b) ((a)>(b)?(a):(b))

typedef struct {
    float x, y;
    float r, g, b;
} vertex_t;
typedef kvec_t(vertex_t) vertices_v;


static GLuint vertex_shader, fragment_shader, program;

static char* vertex_shader_text;
static char* fragment_shader_text;

static GLint mvp_location, vpos_location, vcol_location;
static mat4x4 m, p, mvp;

static GLint vertex_buffer = 0;

static void setup_shaders();
static void load_shader(char* filename, char** buf);



void 
wire_init() {

    setup_shaders();

}

void
wire_set_buffer(shapes_v* shapes) {
    
    if(vertex_buffer>0) glDeleteBuffers(1, &vertex_buffer);

    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);    
    glBufferData(GL_ARRAY_BUFFER, 
        vertices[i].n * sizeof(vertex_t), 
        vertices[i].a, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

// draw loop
void 
wire_draw(shapes_v* shapes, float ratio, float x, float y, float scale) {

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
    mat4x4_translate_in_place(m, posx, posy, 0.0);
    mat4x4_scale_aniso(m, m, scale, scale, scale);

    mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
    mat4x4_mul(mvp, p, m);

    // SET SHADER
    glUseProgram(program);
    // send view matrix
    glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);


    // DRAW
    glDrawArrays(GL_LINE_LOOP, 0, vertices[current_buffer].n);


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
