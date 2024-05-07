#include "glshader.h"

using namespace std;

bool _buildShader(GLuint &id, int &status, string &statusLog, int type, const string &shaderCode) {
    id = glCreateShader(type);

    if (id == 0) {
        statusLog = "_buildShader() failed call to glCreateShader()";
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

        statusLog = "_buildShader() failed call to glCompileShader(): " + string(errorText);

        glDeleteShader(id);
        id = 0;
        delete[] errorText;

        return false;
    }

    return true;
}
