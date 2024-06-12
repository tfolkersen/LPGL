#pragma once

#include <GL/glew.h>
#include <string>

#include "gltexture.h"

enum GLframebufferEnum {
    GLFRAMEBUFFER_ERROR = -1,
    GLFRAMEBUFFER_EMPTY = 0,
    GLFRAMEBUFFER_OK = 1,
};

struct GLframebuffer {
    GLuint id;
    GLframebufferEnum status;
    std::string statusLog;

    // no static factory functions unlike GLtexture and others

    GLframebuffer();
    ~GLframebuffer();

    GLframebuffer(const GLframebuffer &other) = delete;
    GLframebuffer &operator=(const GLframebuffer &other) = delete;

    GLframebuffer(GLframebuffer &&other) noexcept;
    GLframebuffer &operator=(GLframebuffer &&other) noexcept;

    void release();
    void cleanup();

    friend std::ostream &operator<<(std::ostream &os, const GLframebuffer &fb);
    friend std::ostream &operator<<(std::ostream &os, GLframebuffer &fb);
    
    void build(bool _doCleanup = true);

    bool complete();
    GLenum fbstatus();

    void attachColor(const GLtexture &tex);
    void detachColor();
};
