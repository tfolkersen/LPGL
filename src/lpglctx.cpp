#include "lpglctx.h"
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
}

void LPGLctx::freeGLData() {
}
