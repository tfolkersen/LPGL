#include "lpglctx.h"
#include "glstuff.h"
#include <GLFW/glfw3.h>
#include <cmath>
#include <cstdlib>
#include <glm/ext/matrix_transform.hpp>
#include <iostream>
#include <chrono>
#include <functional>
#include "utils.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>

extern "C" {
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

using namespace std;
using glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4, glm::rotate, glm::scale, glm::translate, glm::radians;

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
    float angleDelta = 2.0f;

    float sx = 1.0;
    float sy = 1.0;
    float sd = 0.08;

    float cx = 0.0;
    float cy = 0.0;
    float cd = 3.0;


    float fpx = 0.0;
    float fpy = 0.0;
    float dfpx = 0.08;
    float dfpy = 0.08;

    float fpa = 0.0;
    float dfpa = 0.5;


    float fpScale = 1.0;
    float dfpScale = 0.1;


    float cameraAngle = 0.0f;
    float cameraAngleDelta = 0.5f;
    cameraAngleDelta = 2.0f;

    float cameraScale = 1.0f;

    _mainLoop = [&]() {
        glClear(GL_COLOR_BUFFER_BIT);

        if (glfwGetKey(ctx->window, GLFW_KEY_Q) == GLFW_PRESS) {
            angle += angleDelta;
        }

        if (glfwGetKey(ctx->window, GLFW_KEY_E) == GLFW_PRESS) {
            angle -= angleDelta;
        }

        if (glfwGetKey(ctx->window, GLFW_KEY_Z) == GLFW_PRESS) {
            cameraAngle += cameraAngleDelta;
        }

        if (glfwGetKey(ctx->window, GLFW_KEY_X) == GLFW_PRESS) {
            cameraAngle -= cameraAngleDelta;
        }


        if (glfwGetKey(ctx->window, GLFW_KEY_UP) == GLFW_PRESS) {
            cy += cd;
        }

        if (glfwGetKey(ctx->window, GLFW_KEY_DOWN) == GLFW_PRESS) {
            cy -= cd;
        }

        if (glfwGetKey(ctx->window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
            cx += cd;
        }

        if (glfwGetKey(ctx->window, GLFW_KEY_LEFT) == GLFW_PRESS) {
            cx -= cd;
        }


        if (glfwGetKey(ctx->window, GLFW_KEY_HOME) == GLFW_PRESS) {
            sx += sd;
            sy += sd;
        }

        if (glfwGetKey(ctx->window, GLFW_KEY_END) == GLFW_PRESS) {
            sx -= sd;
            sy -= sd;
        }


        if (glfwGetKey(ctx->window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
            fpScale += dfpScale;
        }

        if (glfwGetKey(ctx->window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
            fpScale -= dfpScale;
        }


        /*
        ctx->setCamera();
        ctx->drawPoly({0.0, 0.0, 0.0, 100.0, 100.0, 100.0}, angle, 1.0, 1.0);
        ctx->drawPoly({0.0, 0.0, 0.0, 100.0, 100.0, 100.0}, angle, 1.5, 1.5);
        ctx->drawPoly({0.0, 0.0, 0.0, 100.0, 100.0, 100.0}, angle, 0.5, 1.5);

        ctx->drawPoly({120, 120, 190, 180, 170, 60}, 0.0, 1.0, 1.0);
        ctx->drawPoly({120, 120, 190, 180, 170, 60}, 0.0, 1.0, 1.0);
*/
        //ctx->setCamera(0.0f, 0.0f, cameraAngle, cameraScale, cameraScale);


        //cin >> cameraScale;

        //cin >> cameraScale; //Use 0.2
        //cameraScale = 0.5;

        //ctx->setCamera(cx, cy, cameraAngle, sx, sy);
        /*
        //ctx->setCamera(0.0f, 0.0f, 0.0f, cameraScale, cameraScale);
        //ctx->drawPoly({0.0, 0.0, 0.0, 150.0, 150.0, 150.0}, angle, 1.0, 1.0);
        //ctx->drawPoly({0.0, 0.0, 0.0, 100.0, 100.0, 100.0}, angle, 0.5, 1.5);

        ctx->drawPoly({0.0, 0.0, 0.0, 100.0, 100.0, 100.0}, angle, 1.0, 1.0);
        ctx->drawPoly({0.0, 0.0, 0.0, 100.0, 100.0, 100.0}, angle, 0.5, 0.5);
        ctx->drawPoly({0.0, 0.0, 0.0, 100.0, 100.0, 100.0}, angle, 0.8, 3);


        //ctx->drawPoly({50.0, 50.0, 50.0, 150.0, 150.0, 150.0}, angle, 1.0, 1.0);

        //cameraAngle = 90.0;

        //cin >> cameraAngle;
        ctx->drawPoly({50.0, 50.0, 50.0, 150.0, 150.0, 150.0}, angle, 0.5, 0.5);
        ctx->drawPoly({50.0, 50.0, 50.0, 150.0, 150.0, 150.0}, angle, 1.0, 1.0);
        */

        //ctx->drawPoly({120, 120, 190, 180, 170, 60}, 0.0, 1.0, 1.0);
        //cout << cameraScale << endl;

        static GLfloat cameraScaleDelta = 0.02f;
        cameraScale += cameraScaleDelta;

        GLfloat _low = 0.05f;
        GLfloat _high = 2.0f;

        if (cameraScale < _low || cameraScale > _high) {
            cameraScaleDelta *= -1.0f;

            cameraScale = constrain(_low, cameraScale, _high);
        }

        chrono::time_point start = chrono::high_resolution_clock::now();

        //for (int i = 0; i < 10000; i++) {
        auto getTime = [&]() -> int {
            chrono::time_point now = chrono::high_resolution_clock::now();
            return chrono::duration_cast<chrono::milliseconds>(now - start).count();
        };


        static unsigned int minCount = ((unsigned int) 0) -1;

        int elapsed = 0;
        int count = 0;
        static double countAvg = 0;
        static int frameNo = 1;

        static int bit = 0;

        if (frameNo % 4 == 0) {
            bit = (bit + 1) % 64;
        }

        ctx->setFillp(((uint64_t) -1) >> (63 - bit));
        ctx->setFillp(0x0000000000042f2);


        //cout << bit << endl;

        static float dx = 0.0;
        //const float ddx = 4.1;
        const float ddx = 4.1;



        if (glfwGetKey(ctx->window, GLFW_KEY_KP_4) == GLFW_PRESS) {
            dx -= ddx;
        }

        if (glfwGetKey(ctx->window, GLFW_KEY_KP_6) == GLFW_PRESS) {
            dx += ddx;
        }




        ctx->anim();



        ctx->setCamera(cx, cy, cameraAngle, sx, sy);

        //ctx->setfpt(fpx, fpy, fpa, fpScale, fpScale);
        ctx->fpt = scale(glm::identity<mat3>(), vec2(1.0 / fpScale, 1.0 / fpScale));

        while ((elapsed = getTime()) < 1000 / 30) {
            //ctx->drawPoly({randf() * 200.0f, randf() * 200.0f, randf() * 200.0f, randf() * 200.0f, randf() * 200.0f, randf() * 200.0f}, angle, 1.0, 1.0);
            //ctx->drawPoly({}, angle, 1.0, 1.0);
            //ctx->drawPoly({}, angle, 1.0, 1.0);
            count++;


            float dy = 0.0;


            //dx = cos((frameNo % 120) / 120.0 * 3.141592654) * 60.0;

            //cout << dx << endl;



            //ctx->drawPoly({dx + 0.0, 0.0, dx + 0.0, 199.0, dx + 199.0, 199.0}, angle, 1.0, 1.0);
            //ctx->drawPoly({dx + 0.0, 0.0, dx + 0.0, 199.0, dx + 199.0, 199.0}, angle, 4.0, 4.0);
            //ctx->drawPoly({0.0, 50.0, 0.0, 150.0, 150.0, 150.0}, angle, 1.0, 1.0);

            //ctx->drawTest({3.0, 3.0}, angle, 1.0, 1.0);
            //ctx->drawTest({199.0, 199.0}, angle, 1.0, 1.0);


            //ctx->drawTest({50.0, 50.0, 50.0, 100.0, 100.0, 50.0}, angle, 5.0, 5.0);
            //ctx->drawTest({0.0, 0.0, 0.0, 100.0, 100.0, 0.0}, angle, 2.0, 2.0);
            //ctx->drawTest({50.0, 50.0, 50.0, 100.0, 100.0, 50.0}, 180,8 2.0, 2.0);
            //dx = angle * 10.0;
            //ctx->drawTest({50.0 + dx, 50.0, 50.0 + dx, 100.0, 100.0 + dx, 50.0}, angle, 6.0, 6.0);
            //ctx->drawTest({}, 0.0, 1.0, 1.0);


            ctx->drawTest({0.0f + dx, 0.0f, 0.0f + dx, 100.0f, 100.0f + dx, 0.0f}, angle, 1.0, 1.0);
            //ctx->drawTest({0.0, 0.0}, 0, 1.0, 1.0);
        }

        minCount = min(minCount, (unsigned int) count);


        countAvg = ((countAvg * (frameNo - 1)) + (double) count) / ((double) frameNo);
        cout << count << " triangles in " << elapsed << " ms " << countAvg << " (average) " << minCount << " (min) "<< endl;
        frameNo++;

        glFinish();

        /*
           ~306k

           ~350k when computing a,b,c before gl

           ~350k+ with more efficient mx/my/up/right

           ~375k+ using vshader and 2D transforms


           */

        //angle += 0.4f;

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
