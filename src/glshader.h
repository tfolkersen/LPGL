#pragma once
#include <GL/glew.h>
#include <string>
#include <ostream>

enum GLSHADER_CONSTANTS {
    GLSHADER_ERROR = -1,
    GLSHADER_DELETED = 0,
    GLSHADER_OK = 1,
};


/*
struct GLvshader {
    GLuint id;
    int status;
    std::string statusLog;

    static GLvshader fromString(const std::string &shaderCode);
    static GLvshader fromFile(const std::string &fileName);

  private:
    GLvshader();
};
*/

struct GLfshader {
    GLuint id;
    int status;
    std::string statusLog;

    static GLfshader fromString(const std::string &shaderCode);
    static GLfshader fromFile(const std::string &fileName);

    friend std::ostream &operator<<(std::ostream &os, GLfshader &shader);

    ~GLfshader();

  private:
    GLfshader();
    void _buildFromString(const std::string &shaderCode);
    void _buildFromFile(const std::string &fileName);
    void cleanup();
};



std::ostream &operator<<(std::ostream &os, GLfshader &shader);
