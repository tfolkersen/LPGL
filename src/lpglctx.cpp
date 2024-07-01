#include "lpglctx.h"
#include "glshader.h"
#include <GLFW/glfw3.h>
#include <iostream>

using namespace std;



GLctx::GLctx() {
}

GLctx::~GLctx() {
    cout << "GLCTX DEST" << endl;
    if (window != NULL) {
        glfwDestroyWindow(window);
    }
}

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
    cout << "LPGLCTX DEST" << endl;
    cleanup();
}

void LPGLctx::cleanup() {
    makeCurrent();

    if (status == LPGLCTX_EMPTY) {
        return;
    }

    if (status == LPGLCTX_OK) {
        freeGLData();
    }
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

 ////////////////////////////// Private functions

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
