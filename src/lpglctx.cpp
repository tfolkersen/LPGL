#include "lpglctx.h"
#include "glshader.h"
#include "gltexture.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <cstring>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/ext.hpp>

using namespace std;
using glm::vec2, glm::vec3, glm::vec4, glm::mat2, glm::mat3, glm::mat4, glm::rotate, glm::scale, glm::translate, glm::radians;
using glm::identity;


bool LPGLctx::exitHandled = false;

GLctx::GLctx() {
}

GLctx::~GLctx() {
    if (window != NULL) {
        glfwDestroyWindow(window);
    }
}

void lpglExitHandler() {
    glfwTerminate();
}

// Private constructor
LPGLctx::LPGLctx() {
    if (!exitHandled) {
        atexit(lpglExitHandler);
        exitHandled = true;
    }

    status = LPGLCTX_EMPTY;
}

unique_ptr<LPGLctx> LPGLctx::fromParameters(const int w, const int h, const char *windowTitle) {
    unique_ptr<LPGLctx> ptr(new LPGLctx());
    ptr->buildFromParameters(w, h, windowTitle);
    return ptr;
}

void LPGLctx::terminate() {
    glfwTerminate();
}

LPGLctx::~LPGLctx() {
    cleanup();
}

ostream &operator<<(ostream &os, const LPGLctx &ctx) {
    os << "LPGLctx " << ((int *) ctx.window) << " " << ctx.status << " | " << ctx.statusLog << " |";
    return os;
}

ostream &operator<<(ostream &os, LPGLctx &ctx) {
    os << "LPGLctx " << ((int *) ctx.window) << " " << ctx.status << " | " << ctx.statusLog << " |";
    return os;
}

void LPGLctx::makeCurrent() {
    if (status != LPGLCTX_OK) {
        return;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        cerr << "Warning: LPGLctx::makeCurrent() failed call to glewInit()" << endl;
    }
}

GLuint LPGLctx::newGlBuffer() {
    GLuint buff;
    glGenBuffers(1, &buff);

    if (buff <= 0) {
        cerr << "LPGLctx newGlBuffer() failed call to glGenBuffers(), got: " << buff << endl;
    }

    glBuffers.insert(buff);

    return buff;
}

GLuint LPGLctx::newGlVertexArray() {
    GLuint vao;
    glGenVertexArrays(1, &vao);

    if (vao <= 0) {
        cerr << "LPGLctx newGlVertexArray() failed call to glGenVertexArrays(), got: " << vao << endl;
    }

    glVertexArrays.insert(vao);

    return vao;
}

bool LPGLctx::deleteGlBuffer(GLuint buff) {
    auto it = glBuffers.find(buff);

    bool found = false;
    if (it != glBuffers.end()) {
        found = true;
        glBuffers.erase(it);
    }

    glDeleteBuffers(1, &buff);

    return found;
}

bool LPGLctx::deleteGlVertexArray(GLuint vao) {
    auto it = glVertexArrays.find(vao);

    bool found = false;
    if (it != glVertexArrays.end()) {
        found = true;
        glVertexArrays.erase(it);
    }

    glDeleteVertexArrays(1, &vao);

    return found;
}

void LPGLctx::cleanup() {
    if (status == LPGLCTX_OK) {
        makeCurrent();
        freeGLData();
    }
}

void LPGLctx::buildFromParameters(const int w, const int h, const char *windowTitle) {
    if (glfwInit() != GLFW_TRUE) {
        status = LPGLCTX_ERROR;
        statusLog = "buildFromParameters() failed call to glfwInit()";
        cerr << *this << endl;
        return;
    }

    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    window = glfwCreateWindow(w, h, windowTitle, NULL, NULL);

    if (window == NULL) {
        status = LPGLCTX_ERROR;
        statusLog = "buildFromParameters() failed call to glfwCreateWindow()";
        cerr << *this << endl;
        return;
    }
    
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    status = LPGLCTX_OK;
    statusLog = "Success";

    makeCurrent();
    initGLData();
}

void LPGLctx::initGLData() {
    const GLfloat fullBox_data[] = {
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
        1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 2.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 3.0f,
    };
    
    const GLuint boxLike_elems[] = {
        0, 1, 2,
        2, 3, 0,
    };

    const GLuint triLike_elems[] = {
        0, 1, 2,
    };

    fullBox_vbuff = newGlBuffer();
    glBindBuffer(GL_ARRAY_BUFFER, fullBox_vbuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullBox_data), fullBox_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    stream_vbuff = newGlBuffer();

    boxLike_ebuff = newGlBuffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxLike_ebuff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxLike_elems), boxLike_elems, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    triLike_ebuff = newGlBuffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triLike_ebuff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triLike_elems), triLike_elems, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    tri_pr = GLprogram::fromFiles("shaders/main/tri.vs", "shaders/main/tri.fs");

    tri_vao = newGlVertexArray();
    glBindVertexArray(tri_vao);
    glBindBuffer(GL_ARRAY_BUFFER, fullBox_vbuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxLike_ebuff);
    glEnableVertexAttribArray(tri_pr.a("a_Pos"));
    glEnableVertexAttribArray(tri_pr.a("a_Color"));
    glEnableVertexAttribArray(tri_pr.a("a_Ordinal"));
    glVertexAttribPointer(tri_pr.a("a_Pos"), 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(tri_pr.a("a_Color"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *) (2 * sizeof(GLfloat)));
    glVertexAttribPointer(tri_pr.a("a_Ordinal"), 1, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *) (5 * sizeof(GLfloat)));
    glBindVertexArray(0);

    // drawPoly
    poly_pr = GLprogram::fromFiles("shaders/main/poly.vs", "shaders/main/poly.fs");

    poly_vao = newGlVertexArray();
    glBindVertexArray(poly_vao);
    glBindBuffer(GL_ARRAY_BUFFER, fullBox_vbuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxLike_ebuff);
    glEnableVertexAttribArray(poly_pr.a("a_Pos"));
    glEnableVertexAttribArray(poly_pr.a("a_Ordinal"));
    glVertexAttribPointer(poly_pr.a("a_Pos"), 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(poly_pr.a("a_Ordinal"), 1, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *) (5 * sizeof(GLfloat)));
    glBindVertexArray(0);

    //cameraMatrix = glm::identity<glm::mat3>();

    model = glm::identity<glm::mat3>();
    camera = glm::identity<glm::mat3>();

    memset(fillp, 0xF, sizeof(fillp));

    test_pr = GLprogram::fromFiles("shaders/main/test.vs", "shaders/main/test.fs");
    test_vao = newGlVertexArray();

    glBindVertexArray(test_vao);
    glBindBuffer(GL_ARRAY_BUFFER, fullBox_vbuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxLike_ebuff);
    glEnableVertexAttribArray(test_pr.a("a_Pos"));
    glEnableVertexAttribArray(test_pr.a("a_Color"));
    glVertexAttribPointer(test_pr.a("a_Pos"), 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glVertexAttribPointer(test_pr.a("a_Color"), 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), (void *) (2 * sizeof(GLfloat)));
    glBindVertexArray(0);


    //drawTest

    //drawSimple
    cam4 = glm::identity<mat4>();
    mod4 = glm::identity<mat4>();
    sim_pr = GLprogram::fromFiles("shaders/main/simple.vs", "shaders/main/simple.fs");

    sim_vao = newGlVertexArray();
    glBindVertexArray(sim_vao);
    glBindBuffer(GL_ARRAY_BUFFER, stream_vbuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, triLike_ebuff);
    glBindVertexArray(0);

    //drawfb
    fb_pr = GLprogram::fromFiles("shaders/main/fb.vs", "shaders/main/fb.fs");

    fb_vao = newGlVertexArray();
    glBindVertexArray(fb_vao);
    glBindBuffer(GL_ARRAY_BUFFER, fullBox_vbuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxLike_ebuff);
    glEnableVertexAttribArray(fb_pr.a("a_Pos"));
    glVertexAttribPointer(fb_pr.a("a_Pos"), 2, GL_FLOAT, GL_FALSE, 6 * sizeof(GLfloat), 0);
    glBindVertexArray(0);




}

void LPGLctx::freeGLData() {
    for (GLuint buff : glBuffers) {
        glDeleteBuffers(1, &buff);
    }
    glBuffers.clear();

    for (GLuint vao : glVertexArrays) {
        glDeleteVertexArrays(1, &vao);
    }
    glVertexArrays.clear();
}

void LPGLctx::drawTri(const vector<GLfloat> &coords) {
    //cout << coords << endl;
    glViewport(0, 0, 200, 200);
    glUseProgram(tri_pr.id);
    glBindVertexArray(tri_vao);


    GLfloat ax = -1;
    GLfloat ay = -1;
    GLfloat bx = -1;
    GLfloat by = -1;
    GLfloat cx = -1;
    GLfloat cy = -1;

    if (coords.size() >= 6) {
        ax = coords[0];
        ay = coords[1];
        bx = coords[2];
        by = coords[3];
        cx = coords[4];
        cy = coords[5];
    }

    glUniform1f(tri_pr.u("u_ax"), ax);
    glUniform1f(tri_pr.u("u_ay"), ay);
    glUniform1f(tri_pr.u("u_bx"), bx);
    glUniform1f(tri_pr.u("u_by"), by);
    glUniform1f(tri_pr.u("u_cx"), cx);
    glUniform1f(tri_pr.u("u_cy"), cy);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void LPGLctx::drawPoly(const vector<GLfloat> &coords, float angle, float scalex, float scaley) {
    //cout << coords << endl;
    glViewport(0, 0, 200, 200);
    glUseProgram(poly_pr.id);
    glBindVertexArray(poly_vao);

    //glUniform1fv(poly_pr.u("u_Tri"), 6, coords.data());
    glUniform1f(poly_pr.u("u_Border"), 2.0f);

    GLfloat center[2];
    center[0] = (coords[0] + coords[2] + coords[4]) / 3.0;
    center[1] = (coords[1] + coords[3] + coords[5]) / 3.0;
    glUniform2fv(poly_pr.u("u_Center"), 1, &center[0]);

    glm::mat3 u_RotScale = glm::identity<glm::mat3>();
    u_RotScale = glm::rotate(u_RotScale, glm::radians(angle));
    u_RotScale = glm::scale(u_RotScale, glm::vec2(scalex, scaley));
    glUniformMatrix3fv(poly_pr.u("u_RotScale"), 1, false, &u_RotScale[0][0]);


    glm::mat3 u_Rot = glm::rotate(glm::identity<glm::mat3>(), glm::radians(angle));
    glUniformMatrix3fv(poly_pr.u("u_Rot"), 1, false, &u_Rot[0][0]);
    glm::mat3 u_Scale = glm::scale(glm::identity<glm::mat3>(), glm::vec2(scalex, scaley));
    glUniformMatrix3fv(poly_pr.u("u_Scale"), 1, false, &u_Scale[0][0]);





    //glUniformMatrix3fv(poly_pr.u("u_Camera"), 1, false, &cameraMatrix[0][0]);


    {
        glm::vec3 a3 = u_RotScale * glm::vec3(coords[0] - center[0], coords[1] - center[1], 1.0);
        glm::vec3 b3 = u_RotScale * glm::vec3(coords[2] - center[0], coords[3] - center[1], 1.0);
        glm::vec3 c3 = u_RotScale * glm::vec3(coords[4] - center[0], coords[5] - center[1], 1.0);

        a3.x += center[0];
        a3.y += center[1];
        b3.x += center[0];
        b3.y += center[1];
        c3.x += center[0];
        c3.y += center[1];

        glUniform2fv(poly_pr.u("a"), 1, &a3[0]);
        glUniform2fv(poly_pr.u("b"), 1, &b3[0]);
        glUniform2fv(poly_pr.u("c"), 1, &c3[0]);

        //cout << a3 << endl;
        //cout << b3 << endl;
        //cout << c3 << endl;
        //cout << endl;

        GLfloat newCenter[2];
        newCenter[0] = (a3.x + b3.x + c3.x) / 3.0;
        newCenter[1] = (a3.y + b3.y + c3.y) / 3.0;
        glUniform2fv(poly_pr.u("u_NewCenter"), 1, &newCenter[0]);

    }


    //glUniform2ui(poly_pr.u("u_Fillp"), fillp[0], fillp[1]);
    

    //                              0        1



    glUniform1iv(poly_pr.u("u_Fillp"), 2, (int32_t *) fillp);


    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

   /*
    glm::vec3 motion = glm::vec3(x, y, 1.0);
    motion = glm::rotate(ident, glm::radians(-angle)) * motion;
    
    camera = glm::identity<glm::mat3>();

    camera = glm::translate(camera, glm::vec2(100.0 + x, 100.0 + y));
    camera = glm::translate(camera, glm::vec2(x, y));
    camera = glm::scale(camera, glm::vec2(1.0 / sx, 1.0 / sy));
    camera = glm::rotate(camera, glm::radians(-angle));



    glm::vec3 rev3 = glm::vec3(-x, -y, 1.0);
    rev3 = glm::scale(ident, glm::vec2(sx, sy)) * glm::rotate(ident, glm::radians(angle)) * rev3;
    glm::vec2 rev2;
    rev2.x = rev3.x;
    rev2.y = rev3.y;

    camera = glm::translate(camera, rev2);

    camera = glm::translate(camera, glm::vec2(motion.x, motion.y));
    
    camera = glm::translate(camera, glm::vec2(-100.0 - x, -100.0 - y));
*/








void LPGLctx::setCamera(GLfloat x, GLfloat y, GLfloat angle, GLfloat sx, GLfloat sy) {
    glm::mat3 ident = glm::identity<glm::mat3>();
    camera = ident;

    vec3 motion = vec3(x, y, 1.0);
    motion = rotate(ident, radians(angle)) * motion;
    motion = scale(ident, vec2(sx, sy)) * motion;

    camera = translate(camera, vec2(100.0 + x, 100.0 + y));
    camera = scale(camera, vec2(1.0 / sx, 1.0 / sy));
    camera = rotate(camera, radians(-angle));
    camera = translate(camera, vec2(-100.0, -100.0));


 }

void LPGLctx::setfpt(GLfloat x, GLfloat y, GLfloat angle, GLfloat sx, GLfloat sy) {
    glm::mat3 ident = glm::identity<glm::mat3>();
    fpt = ident;

    vec3 motion = vec3(x, y, 1.0);
    motion = rotate(ident, radians(angle)) * motion;
    motion = scale(ident, vec2(sx, sy)) * motion;

    fpt = translate(fpt, vec2(100.0 + x, 100.0 + y));
    fpt = scale(fpt, vec2(1.0 / sx, 1.0 / sy));
    fpt = rotate(fpt, radians(-angle));
    fpt = translate(fpt, vec2(-100.0, -100.0));
 }




void LPGLctx::drawTest(const vector<GLfloat> &_coords, float angle, float scalex, float scaley) {
    vector<GLfloat> coords = _coords;

    glViewport(0, 0, 200, 200);
    glUseProgram(test_pr.id);
    glBindVertexArray(test_vao);

    float u_Border = 1.0;
    glUniform1f(test_pr.u("u_Border"), u_Border);
    


    if (coords.size() < 2) {
        cerr << "drawTest needs 2 coordinates, got " << coords.size() << endl;
    }

    int u_DrawCirc = 0;
    if (coords.size() == 2) {
        u_DrawCirc = 1;
        coords.push_back(0.0);
        coords.push_back(0.0);
        coords.push_back(0.0);
        coords.push_back(0.0);
    }

    glUniform1i(test_pr.u("u_DrawCirc"), u_DrawCirc);

    glUniform2fv(test_pr.u("a"), 1, coords.data());
    glUniform2fv(test_pr.u("b"), 1, coords.data() + 2);
    glUniform2fv(test_pr.u("c"), 1, coords.data() + 4);

    glm::vec2 center;
    center.x = (coords[0] + coords[2] + coords[4]) / 3.0f;
    center.y = (coords[1] + coords[3] + coords[5]) / 3.0f;

    glUniform2fv(test_pr.u("u_Center"), 1, &center[0]);


    model = glm::identity<glm::mat3>();
    model = glm::translate(model, center);
    model = glm::scale(model, glm::vec2(1.0 / scalex, 1.0 / scaley));
    model = glm::rotate(model, glm::radians(-angle));
    model = glm::translate(model, -center);

    glUniformMatrix3fv(test_pr.u("model"), 1, false, &model[0][0]);
    glUniformMatrix3fv(test_pr.u("camera"), 1, false, &camera[0][0]);


    glUniform1iv(test_pr.u("u_Fillp"), 2, (int32_t *) fillp);


    glUniformMatrix3fv(test_pr.u("fillpTransform"), 1, false, &fpt[0][0]);




    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}



void LPGLctx::setFillp(uint64_t val) {
    fillp[0] = (val >> 32) & 0xFFFFFFFF;
    fillp[1] = val & 0xFFFFFFFF;
}



void LPGLctx::anim() {
    const float dda = 1.0;
    static float ang = -dda;

    ang += dda;
}

void LPGLctx::drawTex(const GLtexture &tex) {
    glUseProgram(fb_pr.id);

    glBindVertexArray(fb_vao);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex.id);

    glUniform1i(fb_pr.u("tex0"), 0);
    glUniform1f(fb_pr.u("width"), tex.w);
    glUniform1f(fb_pr.u("height"), tex.h);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}


void LPGLctx::drawSimple(std::vector<GLfloat> coords) {
    GLfloat center[2];

    glUseProgram(sim_pr.id);
    glBindVertexArray(sim_vao);

    GLfloat data[] = {
        0.0, 0.0,
        199.0, 199.0,
        199.0, 0.0
    };

    glBindBuffer(GL_ARRAY_BUFFER, stream_vbuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(data), data, GL_STREAM_DRAW);
    glEnableVertexAttribArray(sim_pr.a("a_Pos"));
    glVertexAttribPointer(sim_pr.a("a_Pos"), 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), 0);



    const float dangle = 1.0;
    static float angle = -dangle;
    angle += dangle;


    const float ddx = 0.3;
    static float dx = -ddx;
    dx += ddx;

    const float ddy = 0.15;
    static float dy = -ddy;
    dy += ddy;


    mod4 = identity<mat4>();
    mod4 = translate(mod4, vec3(dx, dy, 0.0));
    mod4 = translate(mod4, vec3(33.333, 33.333, 0.0));
    mod4 = rotate(mod4, glm::radians(angle), vec3(0.0, 0.0, 1.0));
    mod4 = translate(mod4, vec3(-33.333, -33.333, 0.0));

    mod4 = identity<mat4>();

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glLineWidth(2.0);
    //glDisable(GL_BLEND);


    glDrawArrays(GL_TRIANGLES, 0, sizeof(data) / (2 * sizeof(GLfloat)));



    glUniformMatrix4fv(sim_pr.u("model"), 1, false, &mod4[0][0]);
    //glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);

    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_BLEND);


}

