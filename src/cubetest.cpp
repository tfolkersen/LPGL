#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace std;

GLFWwindow *window = NULL;

#define WIN_WIDTH 1024
#define WIN_HEIGHT 768

int main() {
    //init GLFW
    if (glfwInit() != GLFW_TRUE) {
        cerr << "Failed to init GLFW" << endl;
        return -1;
    }

    //make window
    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, "cubetest", NULL, NULL);

    if (window == NULL) {
        cerr << "Failed to make GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    //init GLEW
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to init GLEW" << endl;
        glfwTerminate();
        return -1;
    }


    //init shaders

    //configure drawing

    //draw loop
    while (!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        glClearColor(0.5f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //clean up
    glfwTerminate();

    return 0;
}
