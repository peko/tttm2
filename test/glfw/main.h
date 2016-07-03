
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "linmath.h"
#include "kvec.h"

#include <stdlib.h>
#include <stdio.h>

#define BUFFERS 10
int current_buffer;

GLfloat posx, posy, scale;


void app_init(int argc, char** argv);
void app_draw(float ratio);
void app_cleanup();

void gui_init(GLFWwindow* win);
void gui_logic();
void gui_draw();
void gui_cleanup();

void on_error(int error, const char* description);
void on_key(GLFWwindow* window, int key, int scancode, int action, int mods);
void on_mouse(GLFWwindow* window, double xpos, double ypos);
void on_click(GLFWwindow* window, int button, int action, int mods);
void on_scroll(GLFWwindow* window, double xoffset, double yoffset);
