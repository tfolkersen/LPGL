#pragma once

#include <GL/glew.h>
#include <string>
#include <iostream>
#include "utils.h"
#include <unordered_map>
#include <vector>

/*
    TODO:
    shrink these enums
    add release() functions
    implement GLtexture
*/

enum GLshaderEnum {
    GLSHADER_ERROR = -1,
    GLSHADER_EMPTY = 0,
    GLSHADER_OK = 1,
};

enum GLshaderTypeEnum {
    VSHADER = GL_VERTEX_SHADER,
    FSHADER = GL_FRAGMENT_SHADER,
};

enum GLprogramEnum {
    GLPROGRAM_ERROR = -1,
    GLPROGRAM_EMPTY = 0,
    GLPROGRAM_OK = 1,
};


bool _buildShader(GLuint &id, GLshaderEnum &status, std::string &statusLog, int type, const std::string &shaderCode);

template <GLshaderTypeEnum T>
struct GLshader {
    GLuint id;
    GLshaderEnum status;
    std::string statusLog;
    static const int glType = T;


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

        release();
    }

    void release() {
        id = 0;
        status = GLSHADER_EMPTY;
        statusLog.clear();
    }

    GLshader() {
        id = 0;
        status = GLSHADER_EMPTY;
    }

    ~GLshader() {
        cleanup();
    }


    // Delete: copy constructor, assignment operator
    GLshader(const GLshader<T> &other) = delete;
    GLshader<T> &operator=(const GLshader<T> &other) = delete;

    // Implement move semantics: move constructor, move assignment
    GLshader(GLshader<T> &&other) noexcept {
        id = other.id;
        status = other.status;
        statusLog = move(other.statusLog);

        other.release();
    }

    GLshader<T> &operator=(GLshader<T> &&other) noexcept {
        cleanup();

        id = other.id;
        status = other.status;
        statusLog = move(other.statusLog);

        other.release();

        return *this;
    }

};



struct GLprogram {
    GLuint id;
    GLprogramEnum status;
    std::string statusLog;

    static GLprogram fromStrings(const std::string &vshaderCode, const std::string &fshaderCode);
    static GLprogram fromFiles(const std::string &vsFilename, const std::string &fsFilename);
    static GLprogram fromShaders(const GLshader<VSHADER> &vshader, const GLshader<FSHADER> &fshader);

    void release();

    GLprogram();
    ~GLprogram();

    // Delete: copy constructor, assignment operator
    GLprogram(const GLprogram &other) = delete;
    GLprogram &operator=(const GLprogram &other) = delete;

    // Implement move semantics: move constructor, move assignment
    GLprogram(GLprogram &&other) noexcept;
    GLprogram &operator=(GLprogram &&other) noexcept;


    friend std::ostream &operator<<(std::ostream &os, const GLprogram &program);
    friend std::ostream &operator<<(std::ostream &os, GLprogram &program);

    void cleanup();

    void buildFromStrings(const std::string &vshaderCode, const std::string &fshaderCode);
    void buildFromFiles(const std::string &vsFilename, const std::string &fsFilename);
    void buildFromShaders(const GLshader<VSHADER> &vshader, const GLshader<FSHADER> &fshader);

    GLint a(const std::string &name);
    GLint u(const std::string &name);

    std::vector<GLint> as(const std::vector<std::string> &names);
    std::vector<GLint> us(const std::vector<std::string> &names);

  private:
    std::unordered_map<std::string, GLint> aMap;
    std::unordered_map<std::string, GLint> uMap;
};

typedef GLshader<VSHADER> GLvshader;
typedef GLshader<FSHADER> GLfshader;
