#include "main.h"

static void load_shader(char* filename, char** buf);

GLint mvp_location, vpos_location, vcol_location;

int main(int argc, char** argv) {

    GLFWwindow* window;
    GLuint vertex_shader, fragment_shader, program;

    char* vertex_shader_text;
    char* fragment_shader_text;

    load_shader("shader.vert", &vertex_shader_text);
    load_shader("shader.frag", &fragment_shader_text);

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

    glfwSetKeyCallback(window, on_key);
    glfwSetCursorPosCallback(window, on_mouse);
    glfwSetMouseButtonCallback(window, on_click);
    glfwSetScrollCallback(window, on_scroll);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    posx = 0.0;
    posy = 0.0;
    scale = 1.0;

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

    app_init(argc, argv);
    
    while (!glfwWindowShouldClose(window)) {
        float ratio;
        int width, height;
        mat4x4 m, p, mvp;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        // MVP
        // model coordinates
        // | model matrix m
        // world coordinates
        // | view matrix v
        // camera coordinates
        // | projection matrix p
        // screen coordinates

        mat4x4_identity(m);
        mat4x4_translate_in_place(m, posx, posy, 0.0);
        mat4x4_scale_aniso(m, m, scale,scale,scale);

        // mat4x4_rotate_Z(m, m, mousey/1000.0);
        // mat4x4_rotate_Y(m, m, mousex/1000.0);
        mat4x4_ortho(p, -ratio, ratio, -1.f, 1.f, 1.f, -1.f);
        mat4x4_mul(mvp, p, m);

        // set shader program
        glUseProgram(program);
        glUniformMatrix4fv(mvp_location, 1, GL_FALSE, (const GLfloat*) mvp);

        app_draw();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    app_cleanup();

    free(vertex_shader_text);
    free(fragment_shader_text);

    exit(EXIT_SUCCESS);
}

void setVertexAttr() {
    glEnableVertexAttribArray(vpos_location);
    glVertexAttribPointer(vpos_location, 2, GL_FLOAT, GL_FALSE,
                          sizeof(float) * 5, (void*) 0);

    glEnableVertexAttribArray(vcol_location);
    glVertexAttribPointer(vcol_location, 3, GL_FLOAT, GL_FALSE,
                  sizeof(float) * 5, (void*) (sizeof(float) * 2));
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
