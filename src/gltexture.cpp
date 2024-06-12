#include "gltexture.h"
#include "stb_image.h"
#include <iostream>

using namespace std;

GLtexture GLtexture::fromFile(const string &fileName) {
    GLtexture tex;
    tex.buildFromFile(fileName, false);
    return tex;
}

GLtexture GLtexture::fromDimensions(const int &w, const int &h) {
    GLtexture tex;
    tex.buildFromDimensions(w, h, false);
    return tex;
}

GLtexture::GLtexture() {
    id = 0;
    status = GLTEXTURE_EMPTY;
    w = 0;
    h = 0;
    channels = 0;
}

GLtexture::~GLtexture() {
    cleanup();
}

// Move semantics
GLtexture::GLtexture(GLtexture &&other) noexcept {
    id = other.id;
    status = other.status;
    statusLog = move(other.statusLog);
    w = other.w;
    h = other.h;
    channels = other.channels;

    other.release();
}

GLtexture &GLtexture::operator=(GLtexture &&other) noexcept {
    cleanup();

    id = other.id;
    status = other.status;
    statusLog = move(other.statusLog);
    w = other.w;
    h = other.h;
    channels = other.channels;

    other.release();

    return *this;
}

void GLtexture::release() {
    id = 0;
    status = GLTEXTURE_EMPTY;
    statusLog.clear();
    w = 0;
    h = 0;
    channels = 0;
}

void GLtexture::cleanup() {
    if (status == GLTEXTURE_EMPTY) {
        return;
    }

    if (status == GLTEXTURE_OK) {
        glDeleteTextures(1, &id);
    }

    release();
}

std::ostream &operator<<(std::ostream &os, const GLtexture &tex) {
    os << "GLtexture " << tex.id << " " << tex.status << " | " << tex.statusLog << " |";
    return os;
}

std::ostream &operator<<(std::ostream &os, GLtexture &tex) {
    os << "GLtexture " << tex.id << " " << tex.status << " | " << tex.statusLog << " |";
    return os;
}

void GLtexture::buildFromFile(const string &fileName, bool _doCleanup) {
    if (_doCleanup) {
        cleanup();
    }

    int _channels;
    unsigned char *imgData = stbi_load(fileName.c_str(), &w, &h, &_channels, 4);

    channels = 4;

    if (!imgData) {
        id = 0;
        status = GLTEXTURE_ERROR;
        statusLog = "buildFromFile() failed call to stbi_load() for file " + fileName;
        w = 0;
        h = 0;
        channels = 0;

        cerr << *this << endl;
        return;
    }

    glGenTextures(1, &id);

    if (id == 0) {
        status = GLTEXTURE_ERROR;
        statusLog = "buildFromFile() failed call to glGenTextures() for file " + fileName;
        w = 0;
        h = 0;
        channels = 0;

        cerr << *this << endl;
        stbi_image_free(imgData);
        return;
    }

    // Remember previous: texture handle
    GLint oldBoundTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBoundTexture);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgData);

    // Restore previous texture state
    glBindTexture(GL_TEXTURE_2D, oldBoundTexture);

    status = GLTEXTURE_OK;
    statusLog = "Success";

    stbi_image_free(imgData);
}

void GLtexture::buildFromDimensions(const int &_w, const int &_h, bool _doCleanup) {
    if (_doCleanup) {
        cleanup();
    }

    glGenTextures(1, &id);

    if (id == 0) {
        status = GLTEXTURE_ERROR;
        statusLog = "buildFromDimensions() failed call to glGenTextures()";
        w = 0;
        h = 0;
        channels = 0;

        cerr << *this << endl;
        return;
    }

    w = _w;
    h = _h;
    channels = 4;

    // Remember previous: texture handle
    GLint oldBoundTexture;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &oldBoundTexture);

    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, _w, _h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);

    // Restore previous texture state
    glBindTexture(GL_TEXTURE_2D, oldBoundTexture);

    status = GLTEXTURE_OK;
    statusLog = "Success";
}

