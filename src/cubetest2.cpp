#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "utils.h"
#include "glshader.h"
#include "gltexture.h"


using namespace std;

GLFWwindow *window = NULL;

int main() {

    // Init GLFW
    if (glfwInit() != GLFW_TRUE) {
        cerr << "Failed to init GLFW" << endl;
        return -1;
    }

    // Make a window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 1);

    window = glfwCreateWindow(1024, 768, "cubetest2", NULL, NULL);

    if (window == NULL) {
        cerr << "Failed to make GLFW window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    // Init GLEW
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to init GLEW" << endl;
        glfwTerminate();
        return -1;
    }

    //// Set up drawing stuff

    //Drawing data
    GLfloat vertexData[] = {
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // 7
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // 9
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 3
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 1
    };

    GLuint elementData[] = {
        0, 1, 2,
        2, 3, 0,
    };

    GLuint vbuffer;
    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint ebuffer;
    glGenBuffers(1, &ebuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elementData), elementData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    GLtexture tex0 = GLtexture::fromFile("cubetest.png");
    GLtexture tex1 = GLtexture::fromFile("orange-gradient.png");
    cout << tex0 << endl;
    cout << tex1 << endl;


    // Shaders
    GLvshader vs1 = GLvshader::fromFile("shaders/cubetest2/vs1.vshader");
    GLfshader fs1 = GLfshader::fromFile("shaders/cubetest2/fs1.fshader");
    GLprogram pr1 = GLprogram::fromShaders(vs1, fs1);

    cout << vs1 << endl;
    cout << fs1 << endl;
    cout << pr1 << endl;

    cout << pr1.as({"a_Pos", "a_Color", "a_UV"}) << endl;

    GLprogram pr2 = GLprogram::fromFiles("shaders/cubetest2/vs2.vshader", "shaders/cubetest2/fs2.fshader");
    cout << pr2 << endl;


    // VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);

    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);

    glEnableVertexAttribArray(pr2.a("a_Pos"));
    glVertexAttribPointer(pr2.a("a_Pos"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);

    glEnableVertexAttribArray(pr2.a("a_UV"));
    glVertexAttribPointer(pr2.a("a_UV"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));
    glBindVertexArray(0);

    // Others
    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
    glViewport(0, 0, 1024, 768);

    // Do drawing
    while(!glfwWindowShouldClose(window) && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(pr2.id);
        glUniform1i(pr2.u("u_Tex0"), 0);
        glUniform1i(pr2.u("u_Tex1"), 1);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, tex0.id);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, tex1.id);

        glBindVertexArray(vao);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
