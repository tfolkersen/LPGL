#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

//#include "glshader.h"
//#include "gltexture.h"
//#include "glframebuffer.h"

#include "glstuff.h"

using namespace std;


GLFWwindow *window = NULL;

int main() {
    // init GLFW
    if (glfwInit() != GLFW_TRUE) {
        cerr << "Failed to init GLFW" << endl;
        return -1;
    }

    // make window
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1024, 768, "cubetest2", NULL, NULL);

    if (!window) {
        cerr << "GLFW failed to create window" << endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    // init glew
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to init GLEW" << endl;
        glfwTerminate();
        return -1;
    }

    // drawing data
    GLfloat halfBoxData[] = {
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f, // 7
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f, // 9
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, // 3
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // 1
    };

    GLfloat fullBoxData[] = {
        -1.0f, 1.0f, 0.0f, 0.0f, 1.0f, // 7
        1.0f, 1.0f, 0.0f, 1.0f, 1.0f, // 9
        1.0f, -1.0f, 0.0f, 1.0f, 0.0f, // 3
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f, // 1
    };

    GLuint boxElementData[] = {
        0, 1, 2,
        2, 3, 0,
    };

    GLuint halfBoxBuffer;
    GLuint fullBoxBuffer;
    GLuint boxElementBuffer;

    glGenBuffers(1, &halfBoxBuffer);
    glGenBuffers(1, &fullBoxBuffer);
    glGenBuffers(1, &boxElementBuffer);

    glBindBuffer(GL_ARRAY_BUFFER, halfBoxBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(halfBoxData), halfBoxData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, fullBoxBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullBoxData), fullBoxData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxElementBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxElementData), boxElementData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // programs
    GLprogram pr_main = GLprogram::fromFiles("shaders/cubetest2/vs1.vshader", "shaders/cubetest2/fs1.fshader");
    GLprogram pr_fb = GLprogram::fromFiles("shaders/cubetest2/vs2.vshader", "shaders/cubetest2/fs2.fshader");

    cout << pr_main << endl;
    cout << pr_fb << endl;

    // textures/framebuffers

    GLtexture tex0 = GLtexture::fromFile("cubetest.png");
    GLtexture tex1 = GLtexture::fromFile("orange-gradient.png");
    GLtexture tex_fb = GLtexture::fromDimensions(500, 500);

    cout << tex0 << endl;
    cout << tex1 << endl;
    cout << tex_fb << endl;

    GLframebuffer fb;

    fb.attachColor(tex_fb);



    // draw to fb texture
    cout << "fb complete: " << fb.complete() << endl;

    GLuint vao_fb;
    glGenVertexArrays(1, &vao_fb);

    glBindVertexArray(vao_fb);

    glBindBuffer(GL_ARRAY_BUFFER, fullBoxBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxElementBuffer);

    glEnableVertexAttribArray(pr_fb.a("a_Pos"));
    glEnableVertexAttribArray(pr_fb.a("a_UV"));

    glVertexAttribPointer(pr_fb.a("a_Pos"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glVertexAttribPointer(pr_fb.a("a_UV"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, fb.id);
    glUseProgram(pr_fb.id);
    glViewport(0, 0, tex_fb.w, tex_fb.h);

    glClearColor(1.0f, 0.0f, 1.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex0.id);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex1.id);

    glUniform1i(pr_fb.u("u_Tex0"), 0);
    glUniform1i(pr_fb.u("u_Tex1"), 1);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    // set up drawing to screen

    GLuint vao_main;
    glGenVertexArrays(1, &vao_main);

    glBindVertexArray(vao_main);

    glBindBuffer(GL_ARRAY_BUFFER, halfBoxBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxElementBuffer);

    glEnableVertexAttribArray(pr_main.a("a_Pos"));
    glEnableVertexAttribArray(pr_main.a("a_UV"));

    glVertexAttribPointer(pr_main.a("a_Pos"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glVertexAttribPointer(pr_main.a("a_UV"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
    glUseProgram(pr_main.id);
    glViewport(0, 0, 1024, 768);

    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex_fb.id);

    glUniform1i(pr_main.u("u_Tex0"), 0);


    // main draw loop

    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
    while (glfwWindowShouldClose(window) != GLFW_TRUE && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
