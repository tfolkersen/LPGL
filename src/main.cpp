#include "lpglctx.h"
#include "glstuff.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <functional>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace std;

float randf() {
    return ((float) rand()) / ((float) RAND_MAX);
}

function<void ()> _mainLoop;

void mainLoop() {
    _mainLoop();
}

int main() {
    {
        cout << "Running Lua code, should print a message..." << endl;
        lua_State *L = luaL_newstate();

        luaL_openlibs(L);
        string code = "print(\"Hello from Lua: \" .. (6 + 1 + 4))";
        luaL_loadbuffer(L, code.c_str(), code.size(), "line");
        lua_pcall(L, 0, 0, 0);
        lua_close(L);
        cout << "Done Lua code" << endl;
    }

    //LPGLctxptr ctx = LPGLctx::fromParameters(200, 768, "lpgl-main");
    LPGLctxptr ctx = LPGLctx::fromParameters(200, 200, "lpgl-main");

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearColor(0.3f, 0.3f, 0.3f, 0.0f);

    /*
    _mainLoop = [&]() {
        glClear(GL_COLOR_BUFFER_BIT);

        chrono::time_point start = chrono::high_resolution_clock::now();

        ctx->drawTri({300.0f, 300.0f, 600.0f, 375.0f, 600.0f, 600.0f});
        ctx->drawTri({50.0f, 50.0f, 700.0f, 50.0f, 350.0, 300.0f});

        for (int i = 0; i < 10000; i++) {
            ctx->drawTri({randf() * 200.0f, randf() * 200.0f, randf() * 200.0f, randf() * 200.0f, randf() * 200.0f, randf() * 200.0f});
        }

        glFinish();
        chrono::time_point end = chrono::high_resolution_clock::now();

        auto ms = chrono::duration_cast<chrono::milliseconds>(end - start);


        //cout << "DONE " << ms.count() << endl;

        glfwSwapBuffers(ctx->window);
        glfwPollEvents();
    };
    */


    float angle = 0.0f;
    float sx = 0.0;
    float sy = 0.0;

    _mainLoop = [&]() {
        glClear(GL_COLOR_BUFFER_BIT);

        //ctx->drawPoly({50.0, 50.0, 100.0, 100.0, 150.0, 25.0});
        ctx->drawPoly({0.0, 0.0, 0.0, 100.0, 100.0, 100.0}, angle, 1.0, 1.0);
        ctx->drawPoly({0.0, 0.0, 0.0, 100.0, 100.0, 100.0}, angle, 1.5, 1.5);
        ctx->drawPoly({0.0, 0.0, 0.0, 100.0, 100.0, 100.0}, angle, 2.0, 2.0);

        ctx->drawPoly({120, 120, 190, 180, 170, 60}, 0.0, 1.0, 1.0);











        sx += 0.05;
        sx = sx > 3.0 ? 1.0 : sx;

        sy += 0.05;
        sy = sy > 3.0 ? 1.0 : sy;

        angle += 0.4f;

        glfwSwapBuffers(ctx->window);
        glfwPollEvents();
    };

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(mainLoop, 0, true);
#else
    //TODO make this less jank: should use callback
    while (glfwWindowShouldClose(ctx->window) != GLFW_TRUE && glfwGetKey(ctx->window, GLFW_KEY_ESCAPE) != GLFW_PRESS) {
        mainLoop();
    }
#endif


    return 0;
}
