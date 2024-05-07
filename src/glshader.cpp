#include "glshader.h"
#include "utils.h"
#include <iostream>

using namespace std;

////////////////////////////// Declarations
bool buildShader(GLuint &id, int &status, string &statusLog, int type, const string &shaderCode);


////////////////////////////// GLfshader implementation

GLfshader GLfshader::fromString(const string &shaderCode) {
    GLfshader shader;
    shader._buildFromString(shaderCode);
    return shader;
}


GLfshader GLfshader::fromFile(const string &fileName) {
    GLfshader shader;
    shader._buildFromFile(fileName);
    return shader;
}


ostream &operator<<(ostream &os, GLfshader &shader) {
    os << "GLfshader " << shader.id << " " << shader.status << " | " << shader.statusLog << " |";
    return os;
}

GLfshader::~GLfshader() {
    cleanup();
}

GLfshader::GLfshader() {
}

void GLfshader::_buildFromString(const std::string &shaderCode) {
    bool success = buildShader(id, status, statusLog, GL_FRAGMENT_SHADER, shaderCode);

    if (!success) {
        cerr << *this << endl;
    }
}


void GLfshader::_buildFromFile(const std::string &fileName) {
    string sourceCode;
    bool success = readFile(sourceCode, fileName);

    if (!success) {
        this->id = 0;
        this->status = GLSHADER_ERROR;
        this->statusLog = "_buildFromFile failed at readFile()";
    }

    this->_buildFromString(sourceCode);
}

void GLfshader::cleanup() {
    if (status == GLSHADER_OK) {
        glDeleteShader(id);
    }

    status = GLSHADER_DELETED;
    id = 0;
}



////////////////////////////// File private functions


bool buildShader(GLuint &id, int &status, string &statusLog, int type, const string &shaderCode) {
    id = glCreateShader(type);

    if (id == 0) {
        statusLog = "Failed call to glCreateShader()";
        status = GLSHADER_ERROR;
        return false;
    }

    const char *sourcePtr = shaderCode.c_str();

    glShaderSource(id, 1, &sourcePtr, NULL);

    glCompileShader(id);

    GLint compileStatus;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus == GL_TRUE) {
        status = GLSHADER_OK;
        statusLog = "Success";
        return true;

    } else {
        status = GLSHADER_ERROR;

        GLint errorSize;

        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &errorSize);

        char *errorText = new char[errorSize];
        errorText[errorSize - 1] = 0;

        glGetShaderInfoLog(id, errorSize, NULL, errorText);

        statusLog = "Failed call to glCompileShader(): " + string(errorText);

        glDeleteShader(id);
        id = 0;
        delete[] errorText;

        return false;
    }

    return true;
}



