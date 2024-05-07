#pragma once
#include <GL/glew.h>
#include <string>
#include <ostream>

enum GLSHADER_CONSTANTS {
    GLSHADER_ERROR = -1,
    GLSHADER_DELETED = 0,
    GLSHADER_OK = 1,
    VSHADER = GL_VERTEX_SHADER,
    FSHADER = GL_FRAGMENT_SHADER,
};

template <int T>
struct GLshader {
    static_assert(T == VSHADER || T == FSHADER);

    GLuint id;
    int status;
    std::string statusLog;

    static const int type = T;
    static GLshader<T> fromString(const std::string &shaderCode);
    static GLshader<T> fromFile(const std::string &fileName);

    template <int S>
    friend std::ostream &operator<<(std::ostream &os, GLshader<S> &shader);

    ~GLshader();

  private:
    GLshader();
    void _buildFromString(const std::string &shaderCode);
    void _buildFromFile(const std::string &fileName);
    void cleanup();
};


template <int T>
std::ostream &operator<<(std::ostream &os, GLshader<T> &shader);

