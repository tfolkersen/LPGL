#include "glframebuffer.h"
#include <iostream>

using namespace std;

GLframebuffer::GLframebuffer() {
    build(false);
}

GLframebuffer::~GLframebuffer() {
    cleanup();
}

GLframebuffer::GLframebuffer(GLframebuffer &&other) noexcept {
    id = other.id;
    status = other.status;
    statusLog = move(other.statusLog);

    other.release();
}

GLframebuffer &GLframebuffer::operator=(GLframebuffer &&other) noexcept {
    id = other.id;
    status = other.status;
    statusLog = move(other.statusLog);

    other.release();

    return *this;
}

void GLframebuffer::release() {
    id = 0;
    status = GLFRAMEBUFFER_EMPTY;
    statusLog.clear();
}

void GLframebuffer::cleanup() {
    if (status == GLFRAMEBUFFER_EMPTY) {
        return;
    }

    if (status == GLFRAMEBUFFER_OK) {
        glDeleteFramebuffers(1, &id);
    }

    release();
}

std::ostream &operator<<(std::ostream &os, const GLframebuffer &fb) {
    os << "GLframebuffer " << fb.id << " " << fb.status << " | " << fb.statusLog << " |";
    return os;
}

std::ostream &operator<<(std::ostream &os, GLframebuffer &fb) {
    os << "GLframebuffer " << fb.id << " " << fb.status << " | " << fb.statusLog << " |";
    return os;
}

void GLframebuffer::build(bool _doCleanup) {
    if (_doCleanup) {
        cleanup();
    }

    glGenFramebuffers(1, &id);

    if (id == 0) {
        status = GLFRAMEBUFFER_ERROR;
        statusLog = "GLframebuffer() failed call to glGenFramebuffers()";

        cerr << *this << endl;
        return;
    }

    status = GLFRAMEBUFFER_OK;
    statusLog = "Success";
}

bool GLframebuffer::complete() {
    if (status != GLFRAMEBUFFER_OK) {
        return false;
    }

    return fbstatus() == GL_FRAMEBUFFER_COMPLETE;
}

GLenum GLframebuffer::fbstatus() {
    if (status != GLFRAMEBUFFER_OK) {
        return -1;
    }

    // save previously bound framebuffer
    GLint oldfb;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldfb);

    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
    GLenum result = glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER);

    // restore old state
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oldfb);

    return result;
}

void GLframebuffer::attachColor(const GLtexture &tex) {
    // save previously bound framebuffer, bound texture
    GLint oldfb;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldfb);

    GLint oldBoundTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBoundTexture);

    // attach texture to self
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
    glBindTexture(GL_TEXTURE_2D, tex.id);

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex.id, 0);

    // restore old state
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oldfb);
    glBindTexture(GL_TEXTURE_2D, oldBoundTexture);
}

void GLframebuffer::detachColor() {
    // save previously bound framebuffer, bound texture
    GLint oldfb;
    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &oldfb);

    GLint oldBoundTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBoundTexture);

    // attach 0 texture to self
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, id);
    glBindTexture(GL_TEXTURE_2D, 0);

    glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, 0, 0);

    // restore old state
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, oldfb);
    glBindTexture(GL_TEXTURE_2D, oldBoundTexture);
}

