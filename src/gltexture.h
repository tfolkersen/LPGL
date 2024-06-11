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

    void cleanup();
    void release();

    GLtexture();
    ~GLtexture();

    // Delete move constructor, assignment operator
    GLtexture(const GLtexture &other) = delete;
    GLtexture &operator=(const GLtexture &other) = delete;

    // Move semantics
    GLtexture(GLtexture &&other);
    GLtexture &operator=(GLtexture &&other);

    friend std::ostream &operator<<(std::ostream &os, const GLtexture &tex);
    friend std::ostream &operator<<(std::ostream &os, GLtexture &tex);

    void buildFromFile(const std::string &fileName);
    void buildFromDimensions(const int &w, const int &h);

};





