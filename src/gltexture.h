#pragma once

#include <GL/glew.h>
#include <string>
#include <ostream>

enum GLtextureEnum {
    GLTEXTURE_ERROR = -1,
    GLTEXTURE_EMPTY = 0,
    GLTEXTURE_OK = 1,
};

struct GLtexture {
    GLuint id;
    GLtextureEnum status;
    std::string statusLog;
    int w, h, channels;

    static GLtexture fromFile(const std::string &fileName);
    static GLtexture fromDimensions(const int &w, const int &h);

    GLtexture();
    ~GLtexture();

    // Delete move constructor, assignment operator
    GLtexture(const GLtexture &other) = delete;
    GLtexture &operator=(const GLtexture &other) = delete;

    // Move semantics
    GLtexture(GLtexture &&other) noexcept;
    GLtexture &operator=(GLtexture &&other) noexcept;

    void release();
    void cleanup();

    friend std::ostream &operator<<(std::ostream &os, const GLtexture &tex);
    friend std::ostream &operator<<(std::ostream &os, GLtexture &tex);

    void buildFromFile(const std::string &fileName, bool _doCleanup = true);
    void buildFromDimensions(const int &w, const int &h, bool _doCleanup = true);
};
