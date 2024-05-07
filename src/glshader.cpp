#include "glshader.h"
#include "utils.h"
#include <string>
#include <sys/types.h>

using namespace std;

bool _buildShader(GLuint &id, GLshaderEnum &status, string &statusLog, int type, const string &shaderCode) {
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


////////////////////////////// GLprogram implementation

GLprogram GLprogram::fromStrings(const std::string &vshaderCode, const std::string &fshaderCode) {
    GLprogram program;
    program.buildFromStrings(vshaderCode, fshaderCode);
    return program;
}

GLprogram GLprogram::fromFiles(const std::string &vsFilename, const std::string &fsFilename) {
    GLprogram program;
    program.buildFromFiles(vsFilename, fsFilename);
    return program;
}

GLprogram GLprogram::fromShaders(const GLshader<VSHADER> &vshader, const GLshader<FSHADER> &fshader) {
    GLprogram program;
    program.buildFromShaders(vshader, fshader);
    return program;
}

GLprogram::GLprogram() {
    id = 0;
    status = GLPROGRAM_EMPTY;
    statusLog = "Uninitialized";
}

GLprogram::~GLprogram() {
    cleanup();
}

std::ostream &operator<<(std::ostream &os, const GLprogram &program) {
    os << "GLprogram " << program.id << " " << program.status << " | " << program.statusLog << " |";
    return os;
}

std::ostream &operator<<(std::ostream &os, GLprogram &program) {
    os << "GLprogram " << program.id << " " << program.status << " | " << program.statusLog << " |";
    return os;
}

void GLprogram::cleanup() {
    aMap.clear();
    uMap.clear();

    if (status == GLPROGRAM_EMPTY) {
        return;
    }

    if (status == GLPROGRAM_OK) {
        glDeleteProgram(id);
    }

    id = 0;
    status = GLPROGRAM_EMPTY;
    statusLog = "Uninitialized";
}


void GLprogram::buildFromStrings(const std::string &vshaderCode, const std::string &fshaderCode) {
    cleanup();

    auto vs = GLshader<VSHADER>::fromString(vshaderCode);
    auto fs = GLshader<FSHADER>::fromString(fshaderCode);
    this->buildFromShaders(vs, fs);
}

void GLprogram::buildFromFiles(const std::string &vsFilename, const std::string &fsFilename) {
    cleanup();

    string vsCode;
    string fsCode;

    bool ok1 = readFile(vsCode, vsFilename);

    if (!ok1) {
        id = 0;
        status = GLPROGRAM_ERROR;
        statusLog = "GLprogram buildFromFiles() failed to read vshader file: " + vsFilename;
        cerr << statusLog << endl;

        return;
    }

    bool ok2 = readFile(fsCode, fsFilename);

    if (!ok2) {
        id = 0;
        status = GLPROGRAM_ERROR;
        statusLog = "GLprogram buildFromFiles() failed to read fshader file: " + fsFilename;
        cerr << statusLog << endl;

        return;
    }

    this->buildFromStrings(vsCode, fsCode);
}

void GLprogram::buildFromShaders(const GLshader<VSHADER> &vshader, const GLshader<FSHADER> &fshader) {
    cleanup();

    bool error = false;
    statusLog = "";

    if (vshader.status != GLSHADER_OK) {
        statusLog += "vshader bad status: " + to_string(vshader.status) + " ";
        error = true;
    }

    if (fshader.status != GLSHADER_OK) {
        statusLog += "fshader bad status: " + to_string(fshader.status) + " ";
        error = true;
    }

    if (error) {
        id = 0;
        status = GLPROGRAM_ERROR;
        statusLog = "GLprogram buildFromShaders() error: " + statusLog;

        cerr << statusLog << endl;
        return;
    }

    id = glCreateProgram();

    if (id == 0) {
        status = GLPROGRAM_ERROR;
        statusLog = "GLprogram buildFromShaders() failed call to glCreateProgram()";
        cerr << statusLog << endl;
        return;
    }

    glAttachShader(id, vshader.id);
    glAttachShader(id, fshader.id);
    glLinkProgram(id);

    GLint linkStatus;
    glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_TRUE) {
        status = GLPROGRAM_OK;
        statusLog = "Success";
        return;

    } else {
        status = GLPROGRAM_ERROR;

        GLint errorSize;

        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &errorSize);

        char *errorText = new char[errorSize];
        errorText[errorSize - 1] = 0;

        glGetProgramInfoLog(id, errorSize, NULL, errorText);

        statusLog = "GLprogram buildFromShaders() failed call to glLinkProgram(): " + string(errorText);
        cerr << statusLog << endl;

        delete[] errorText;

        glDeleteProgram(id);
        id = 0;
        return;
    }
}




GLint GLprogram::a(const std::string &name) {
    //Check map for value
    auto it = aMap.find(name);

    if (it != aMap.end()) {
        return it->second;
    }

    //Not found, check program
    if (status != GLPROGRAM_OK) {
        return -1;
    }

    GLint val = glGetAttribLocation(id, name.c_str());

    aMap[name] = val;

    return val;
}


GLint GLprogram::u(const std::string &name) {
    //Check map for value
    auto it = uMap.find(name);

    if (it != uMap.end()) {
        return it->second;
    }

    //Not found, check program
    if (status != GLPROGRAM_OK) {
        return -1;
    }

    GLint val = glGetUniformLocation(id, name.c_str());

    uMap[name] = val;

    return val;
}


vector<GLint> GLprogram::as(const vector<string> &names) {
    vector<GLint> vals;

    for (const string &name : names) {
        vals.push_back(a(name));
    }

    return vals;
}


vector<GLint> GLprogram::us(const vector<string> &names) {
    vector<GLint> vals;

    for (const string &name : names) {
        vals.push_back(u(name));
    }

    return vals;
}

