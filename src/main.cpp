#include "lpglctx.h"
#include "glstuff.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <chrono>

using namespace std;

float randf() {
    return ((float) rand()) / ((float) RAND_MAX);
}


int main() {
    //LPGLctxptr ctx = LPGLctx::fromParameters(200, 768, "lpgl-main");
    LPGLctxptr ctx = LPGLctx::fromParameters(1024, 768, "lpgl-main");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //TODO make this less jank
    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);
    while (glfwWindowShouldClose(ctx->window) != GLFW_TRUE && glfwGetKey(ctx->window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        glClear(GL_COLOR_BUFFER_BIT);

        chrono::time_point start = chrono::high_resolution_clock::now();

        ctx->drawTri({300.0f, 300.0f, 600.0f, 375.0f, 600.0f, 600.0f});
        ctx->drawTri({50.0f, 50.0f, 700.0f, 50.0f, 350.0, 300.0f});

        for (int i = 0; i < 10000; i++) {
            ctx->drawTri({randf() * 1024.0f, randf() * 768.0f, randf() * 1024.0f, randf() * 768.0f, randf() * 1024.0f, randf() * 768.0f});
        }

        glFinish();
        chrono::time_point end = chrono::high_resolution_clock::now();

        auto ms = chrono::duration_cast<chrono::milliseconds>(end - start);


        cout << "DONE " << ms.count() << endl;

        glfwSwapBuffers(ctx->window);
        glfwPollEvents();
    }


    ctx->cleanup();
    LPGLctx::terminate();
    return 0;
}
