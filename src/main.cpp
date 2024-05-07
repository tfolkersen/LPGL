#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glshader.h"

using namespace std;

GLFWwindow *window = NULL;

bool init() {
    //init GLFW
    if (glfwInit() != GLFW_TRUE) {
        cerr << "Failed to init GLFW" << endl;
        return false;
    }

    //make window
    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1024, 768, "lpgl-main", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    //init GLEW
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to init GLEW" << endl;
        glfwTerminate();
        return false;
    }

    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

    return true;
}


int main() {
    cout << "HELLO FROM MAIN.CPP" << endl;

    if (!init()) {
        return -1;
    }

    auto vs = GLshader<VSHADER>::fromFile("shaders/cubetest.vshader");
    auto fs = GLshader<FSHADER>::fromFile("shaders/cubetest.fshader");

    cout << vs << endl;
    cout << fs << endl;

    const auto &vss = vs;
    cout << vss << endl;


    //draw loop
    do {
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwWindowShouldClose(window) != GLFW_TRUE && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);

    //clean up
    glfwTerminate();
    cout << "GOODBYE FROM MAIN.CPP" << endl;

    return 0;
}
