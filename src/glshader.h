#pragma once

#include <GL/glew.h>
#include <string>
#include <iostream>
#include "utils.h"

enum GLSHADER_CONSTANTS {
    GLSHADER_ERROR = -1,
    GLSHADER_EMPTY = 0,
    GLSHADER_OK = 1,
    VSHADER = GL_VERTEX_SHADER,
    FSHADER = GL_FRAGMENT_SHADER,
};

enum GLPROGRAM_CONSTANTS {
    GLPROGRAM_ERROR = -1,
    GLPROGRAM_EMPTY = 0,
    GLPROGRAM_OK = 1,
};



bool _buildShader(GLuint &id, int &status, std::string &statusLog, int type, const std::string &shaderCode);

template <int T>
struct GLshader {
    static_assert(T == VSHADER || T == FSHADER);

    GLuint id;
    int status;
    std::string statusLog;
    static const int type = T;


    static GLshader<T> fromString(const std::string &shaderCode) {
        GLshader<T> shader;
        shader.buildFromString(shaderCode);
        return shader;
    }

    static GLshader<T> fromFile(const std::string &fileName) {
        GLshader<T> shader;
        shader.buildFromFile(fileName);
        return shader;
    }

    friend std::ostream &operator<<(std::ostream &os, const GLshader<T> &shader) {
        os << "GLshader<" << T << "> " << shader.id << " " << shader.status << " | " << shader.statusLog << " |";
        return os;
    }

    friend std::ostream &operator<<(std::ostream &os, GLshader<T> &shader) {
        os << "GLshader<" << T << "> " << shader.id << " " << shader.status << " | " << shader.statusLog << " |";
        return os;
    }

    ~GLshader() {
        cleanup();
    }

    void buildFromString(const std::string &shaderCode) {
        cleanup();

        bool success = _buildShader(id, status, statusLog, T, shaderCode);

        if (!success) {
            std::cerr << *this << std::endl;
        }
    }

    void buildFromFile(const std::string &fileName) {
        cleanup();

        std::string sourceCode;
        bool success = readFile(sourceCode, fileName);

        if (!success) {
            this->id = 0;
            this->status = GLSHADER_ERROR;
            this->statusLog = "buildFromFile() failed at readFile()";

            std::cerr << *this << std::endl;
            return;
        }

        this->buildFromString(sourceCode);
    }

    void cleanup() {
        if (status == GLSHADER_EMPTY) {
            return;
        }

        if (status == GLSHADER_OK) {
            glDeleteShader(id);
        }

        id = 0;
        status = GLSHADER_EMPTY;
        statusLog = "Uninitialized";
    }

    GLshader() {
        id = 0;
        status = GLSHADER_EMPTY;
        statusLog = "Uninitialized";
    }
};


struct GLprogram {
    GLuint id;
    int status;
    std::string statusLog;

    static GLprogram fromStrings(const std::string &vshaderCode, const std::string &fshaderCode);
    static GLprogram fromFiles(const std::string &vsFilename, const std::string &fsFilename);
    static GLprogram fromShaders(const GLshader<VSHADER> &vshader, const GLshader<FSHADER> &fshader);

    GLprogram();
    ~GLprogram();

    friend std::ostream &operator<<(std::ostream &os, const GLprogram &program);
    friend std::ostream &operator<<(std::ostream &os, GLprogram &program);

    void cleanup();

    void buildFromStrings(const std::string &vshaderCode, const std::string &fshaderCode);
    void buildFromFiles(const std::string &vsFilename, const std::string &fsFilename);
    void buildFromShaders(const GLshader<VSHADER> &vshader, const GLshader<FSHADER> &fshader);
};
