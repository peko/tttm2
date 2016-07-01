#include <stdio.h>
#include <GLFW/glfw3.h>

void on_error(int error, const char* description) {
    fprintf(stderr, "Error: %s\n", description);
}

void on_key(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
}

void on_mouse(GLFWwindow* window, double xpos, double ypos){
}

void on_click(GLFWwindow* window, int button, int action, int mods) {
}

void on_scroll(GLFWwindow* window, double xoffset, double yoffset) {
}

