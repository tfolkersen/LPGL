#include "lpglctx.h"
#include "glshader.h"
#include <GLFW/glfw3.h>
#include <cstdlib>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/common.hpp>
#include <glm/ext.hpp>

using namespace std;

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
        -1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 1.0f, 0.0f, 1.0f,
    };
    
    const GLuint boxLike_elems[] = {
        0, 1, 2,
        2, 3, 0,
    };

    fullBox_vbuff = newGlBuffer();
    glBindBuffer(GL_ARRAY_BUFFER, fullBox_vbuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullBox_data), fullBox_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    boxLike_ebuff = newGlBuffer();
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxLike_ebuff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxLike_elems), boxLike_elems, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    tri_pr = GLprogram::fromFiles("shaders/main/tri.vs", "shaders/main/tri.fs");

    tri_vao = newGlVertexArray();
    glBindVertexArray(tri_vao);
    glBindBuffer(GL_ARRAY_BUFFER, fullBox_vbuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxLike_ebuff);
    glEnableVertexAttribArray(tri_pr.a("a_Pos"));
    glEnableVertexAttribArray(tri_pr.a("a_Color"));
    glVertexAttribPointer(tri_pr.a("a_Pos"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glVertexAttribPointer(tri_pr.a("a_Color"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) (2 * sizeof(GLfloat)));
    glBindVertexArray(0);

    // drawPoly
    poly_pr = GLprogram::fromFiles("shaders/main/poly.vs", "shaders/main/poly.fs");

    poly_vao = newGlVertexArray();
    glBindVertexArray(poly_vao);
    glBindBuffer(GL_ARRAY_BUFFER, fullBox_vbuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxLike_ebuff);
    glEnableVertexAttribArray(poly_pr.a("a_Pos"));
    glVertexAttribPointer(poly_pr.a("a_Pos"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glBindVertexArray(0);

    cameraMatrix = glm::identity<glm::mat4>();
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

    glUniform1fv(poly_pr.u("u_Tri"), 6, coords.data());
    glUniform1f(poly_pr.u("u_Border"), 2.0f);

    glUniform1f(poly_pr.u("u_ScaleX"), scalex);
    glUniform1f(poly_pr.u("u_ScaleY"), scaley);

    GLfloat center[2];
    center[0] = (coords[0] + coords[2] + coords[4]) / 3.0;
    center[1] = (coords[1] + coords[3] + coords[5]) / 3.0;
    glUniform2fv(poly_pr.u("u_Center"), 1, &center[0]);

    glm::mat4 u_RotScale = glm::identity<glm::mat4>();
    u_RotScale = glm::rotate(u_RotScale, glm::radians(angle), glm::vec3(0.0, 0.0, 1.0));
    u_RotScale = glm::scale(u_RotScale, glm::vec3(scalex, scaley, 1.0f));
    glUniformMatrix4fv(poly_pr.u("u_RotScale"), 1, false, &u_RotScale[0][0]);

    glm::mat4 u_Mat1 = glm::identity<glm::mat4>();

    //u_Mat1 = glm::rotate(u_Mat1, (float) glm::radians(5.0), glm::vec3(0.0, 0.0, 1.0));
    //u_Mat1 = glm::translate(u_Mat1, {-30.0, 0.0, 0.0});



    glUniformMatrix4fv(poly_pr.u("u_Mat1"), 1, false, &u_Mat1[0][0]);

    glUniformMatrix4fv(poly_pr.u("u_Camera"), 1, false, &cameraMatrix[0][0]);


    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

}

void LPGLctx::setCamera(GLfloat x, GLfloat y, GLfloat angle, GLfloat sx, GLfloat sy) {
    cameraMatrix = glm::identity<glm::mat4>();

    cameraMatrix = glm::translate(cameraMatrix, glm::vec3(x, y, 0.0f));
    cameraMatrix = glm::translate(cameraMatrix, glm::vec3(100.0f, 100.0f, 0.0f));
    cameraMatrix = glm::scale(cameraMatrix, glm::vec3(1.0 / sx, 1.0 / sy, 1.0f));
    cameraMatrix = glm::rotate(cameraMatrix, glm::radians(-angle), glm::vec3(0.0f, 0.0f, 1.0f));
    cameraMatrix = glm::translate(cameraMatrix, glm::vec3(-100.0f, -100.0f, 0.0f));

    //cameraMatrix = glm::rotate(cameraMatrix, glm::radians(-angle), glm::vec3(0.0f, 0.0f, -1.0f));
    //cameraMatrix = glm::translate(cameraMatrix, glm::vec3(0.0f, 0.0f, 0.0f));

}
