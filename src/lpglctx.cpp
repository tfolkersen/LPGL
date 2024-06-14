#include "lpglctx.h"
#include "glshader.h"
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;

unique_ptr<LPGLctx> LPGLctx::fromParameters(const int w, const int h, const char *windowTitle) {
    unique_ptr<LPGLctx> ptr(new LPGLctx());
    ptr->buildFromParameters(w, h, windowTitle, false);
    return ptr;
}

void LPGLctx::terminate() {
    glfwTerminate();
}

// Private constructor
LPGLctx::LPGLctx() {
    window = NULL;
    status = LPGLCTX_EMPTY;
}

LPGLctx::~LPGLctx() {
    cleanup();
}

LPGLctx::LPGLctx(LPGLctx &&other) noexcept {
    moveFrom(forward<LPGLctx>(other), false);
}

LPGLctx &LPGLctx::operator=(LPGLctx &&other) noexcept {
    moveFrom(forward<LPGLctx>(other), true);
    return *this;
}

void LPGLctx::release() {
    window = NULL;
    status = LPGLCTX_EMPTY;
    statusLog.clear();
}

void LPGLctx::cleanup() {
    if (status == LPGLCTX_EMPTY) {
        return;
    }

    if (status == LPGLCTX_OK) {
        freeGLData();
        glfwDestroyWindow(window);
    }

    release();
}

ostream &operator<<(ostream &os, const LPGLctx &ctx) {
    os << "LPGLctx " << ((int *) ctx.window) << " " << ctx.status << " | " << ctx.statusLog << " |";
    return os;
}

ostream &operator<<(ostream &os, LPGLctx &ctx) {
    os << "LPGLctx " << ((int *) ctx.window) << " " << ctx.status << " | " << ctx.statusLog << " |";
    return os;
}

void LPGLctx::buildFromParameters(const int w, const int h, const char *windowTitle, bool _doCleanup) {
    if (_doCleanup) {
        cleanup();
    }

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

void LPGLctx::makeCurrent() {
    if (status != LPGLCTX_OK) {
        return;
    }

    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) {
        cerr << "Warning: LPGLctx::makeCurrent() failed call to glewInit()" << endl;
    }
}


 ////////////////////////////// Private functions
void LPGLctx::moveFrom(LPGLctx &&other, bool _doCleanup) {
    if (_doCleanup) {
        cleanup();
    }

    window = other.window;
    status = other.status;
    statusLog = move(other.status);

    other.release();
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

    glGenBuffers(1, &fullBox_vbuff);
    glBindBuffer(GL_ARRAY_BUFFER, fullBox_vbuff);
    glBufferData(GL_ARRAY_BUFFER, sizeof(fullBox_data), fullBox_data, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &boxLike_ebuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxLike_ebuff);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(boxLike_elems), boxLike_elems, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    tri_pr = GLprogram::fromFiles("shaders/main/tri.vs", "shaders/main/tri.fs");

    glGenVertexArrays(1, &tri_vao);
    glBindVertexArray(tri_vao);
    glBindBuffer(GL_ARRAY_BUFFER, fullBox_vbuff);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxLike_ebuff);
    glEnableVertexAttribArray(tri_pr.a("a_Pos"));
    glEnableVertexAttribArray(tri_pr.a("a_Color"));
    glVertexAttribPointer(tri_pr.a("a_Pos"), 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), 0);
    glVertexAttribPointer(tri_pr.a("a_Color"), 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *) (2 * sizeof(GLfloat)));
    glBindVertexArray(0);

}

void LPGLctx::freeGLData() {
    glDeleteBuffers(1, &fullBox_vbuff);

    glDeleteBuffers(1, &boxLike_ebuff);

    tri_pr.cleanup();

    glDeleteVertexArrays(1, &tri_vao);
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
