#include "main.h"

int main(int argc, char** argv) {

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

    glfwSetKeyCallback(window, on_key);
    glfwSetCursorPosCallback(window, on_mouse);
    glfwSetMouseButtonCallback(window, on_click);
    glfwSetScrollCallback(window, on_scroll);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
    glfwSwapInterval(1);

    app_init(argc, argv);
    gui_init(window);

    while (!glfwWindowShouldClose(window)) {
        float ratio;
        int width, height;

        glfwGetFramebufferSize(window, &width, &height);
        ratio = width / (float) height;

        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwPollEvents();

        app_draw(ratio);

        gui_logic();
        gui_draw();

        glfwSwapBuffers(window);
    }

    glfwDestroyWindow(window);

    glfwTerminate();

    app_cleanup();
    gui_cleanup();

    exit(EXIT_SUCCESS);
}
