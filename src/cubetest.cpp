#include <cstdio>
#include <iostream>

#include <vector>
#include <unordered_map>


#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace std;

//////////////////////////////////////////////////////////// UTILS
string readFile(const string &fileName) {
    string fileData;

    FILE *f = fopen(fileName.c_str(), "r");

    if (f == NULL) {
        cerr << "Failed to open file: " << fileName << endl;
        return fileData;
    }

    fseek(f, 0, SEEK_END);
    long fileSize = ftell(f);
    fseek(f, 0, SEEK_SET);

    fileData.resize(fileSize);

    fread(fileData.data(), 1, fileSize, f);

    fclose(f);

    return fileData;
}

//////////////////////////////////////////////////////////// SHADERS
enum SHADERSTATUS {
    SHADER_ERROR = -1,
    SHADER_DELETED = 0,
    SHADER_OK = 1,
};

struct Shader {
    GLuint id;
    GLenum type;
    int status;
    std::string statusLog;

    Shader(const std::string &shaderCode, GLenum type);
    void cleanup();
};

Shader::Shader(const string &shaderCode, GLenum type) {
    id = 0;
    this->type = type;

    id = glCreateShader(type);

    if (id == 0) {
        statusLog = "Failed call to glCreateShader()";
        status = SHADER_ERROR;
        cerr << statusLog << endl;
        return;
    }

    const char *sourcePtr = shaderCode.c_str();

    glShaderSource(id, 1, &sourcePtr, NULL);

    glCompileShader(id);

    GLint compileStatus;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);

    if (compileStatus == GL_TRUE) {
        status = SHADER_OK;
        statusLog = "Success";
    } else {
        status = SHADER_ERROR;

        GLint logSize;

        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logSize);
        statusLog.resize(logSize - 1);

        glGetShaderInfoLog(id, statusLog.size() + 1, NULL, statusLog.data());

        cerr << "Failed call to glCompileShader(): " << statusLog << endl;

        glDeleteShader(id);
        id = 0;
    }
}

void Shader::cleanup() {
    if (status == SHADER_OK) {
        glDeleteShader(id);
    }

    status = SHADER_DELETED;
    id = 0;
}

struct ShaderProgram {
    GLuint id;
    int status;
    std::string statusLog;

    ShaderProgram(const Shader &vshader, const Shader &fshader);
    ShaderProgram(const std::string &vshaderSource, const std::string &fshaderSource);
    void cleanup();

    void findAttribute(const std::string &name);
    void findUniform(const std::string &name);

    void findAttributes(const std::vector<std::string> &names);
    void findUniforms(const std::vector<std::string> &names);

    GLint operator[](const std::string &name);


  private:
    void compile(const Shader &vshader, const Shader &fshader);
    //std::unordered_map<const char *, GLint> locationMap;
    std::unordered_map<std::string, GLint> locationMap;


};

void ShaderProgram::compile(const Shader &vshader, const Shader &fshader) {
    if (vshader.type != GL_VERTEX_SHADER) {
        cerr << "Warning: ShaderProgram() vshader parameter is of type " << vshader.type << endl;
    }

    if (fshader.type != GL_FRAGMENT_SHADER) {
        cerr << "Warning: ShaderProgram() fshader parameter is of type " << fshader.type << endl;
    }

    bool err = false;

    if (vshader.status != SHADER_OK) {
        cerr << "Error: ShaderProgram() vshader parameter status is " << vshader.status << endl;
        err = true;
    }

    if (fshader.status != SHADER_OK) {
        cerr << "Error: ShaderProgram() fshader parameter status is " << fshader.status << endl;
        err = true;
    }

    if (err) {
        status = SHADER_ERROR;
        statusLog = "vshader/fshader status not SHADER_OK";
        id = 0;
        return;
    }

    id = glCreateProgram();

    if (id == 0) {
        status = SHADER_ERROR;
        statusLog = "Failed call to glCreateProgram()";
        cerr << statusLog << endl;
        return;
    }

    glAttachShader(id, vshader.id);
    glAttachShader(id, fshader.id);
    glLinkProgram(id);

    GLint linkStatus;
    glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_TRUE) {
        status = SHADER_OK;
        statusLog = "Success";
    } else {
        status = SHADER_ERROR;

        GLint logSize;

        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logSize);

        statusLog.resize(logSize - 1);

        glGetProgramInfoLog(id, statusLog.size() + 1, NULL, statusLog.data());

        cerr << "Failed call to glLinkProgram(): " << statusLog << endl;

        glDeleteProgram(id);
        id = 0;
    }
}


ShaderProgram::ShaderProgram(const Shader &vshader, const Shader &fshader) {
    compile(vshader, fshader);
}


ShaderProgram::ShaderProgram(const string &vshaderSource, const string &fshaderSource) {
    Shader vshader(vshaderSource, GL_VERTEX_SHADER);
    Shader fshader(fshaderSource, GL_FRAGMENT_SHADER);

    compile(vshader, fshader);

    vshader.cleanup();
    fshader.cleanup();
}


void ShaderProgram::cleanup() {
    if (status == SHADER_OK) {
        glDeleteProgram(id);
    }

    id = 0;
    status = SHADER_DELETED;

    locationMap.clear();
}


void ShaderProgram::findAttribute(const string &name) {
    if (status != SHADER_OK) {
        return;
    }

    GLint location = glGetAttribLocation(id, name.c_str());

    if (location == -1) {
        cerr << "ShaderProgram failed to find attribute \"" << name << "\"" << endl;
        return;
    } else {
        locationMap[name] = location;
    }
}

void ShaderProgram::findUniform(const string &name) {
    if (status != SHADER_OK) {
        return;
    }

    GLint location = glGetUniformLocation(id, name.c_str());

    if (location == -1) {
        cerr << "ShaderProgram failed to find uniform \"" << name << "\"" << endl;
        return;
    } else {
        locationMap[name] = location;
    }
}


void ShaderProgram::findAttributes(const vector<string> &names) {
    for (const string &name : names) {
        findAttribute(name);
    }
}

void ShaderProgram::findUniforms(const vector<string> &names) {
    for (const string &name : names) {
        findUniform(name);
    }
}



GLint ShaderProgram::operator[](const std::string &name) {
    auto it = locationMap.find(name);

    if (it == locationMap.end()) {
        return -1;
    }

    return it->second;
}





////////////////////////////////////////////////////////////


GLFWwindow *window = NULL;


int main() {
    //init GLFW
    if (glfwInit() != GLFW_TRUE) {
        cerr << "Failed to init GLFW" << endl;
        return -1;
    }

    //make window
    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(1024, 768, "cubetest", NULL, NULL);
    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    //init GLEW
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to init GLEW" << endl;
        glfwTerminate();
        return -1;
    }

    //buffer data
    GLfloat vertexData[] = {
        -0.5f, -0.5f, 0.0f, 0.5f, 0.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
    };

    GLuint vbuffer;
    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    //set up shader program
    string vshaderCode = readFile("shaders/cubetest.vshader");
    string fshaderCode = readFile("shaders/cubetest.fshader");

    Shader vshader(vshaderCode, GL_VERTEX_SHADER);
    Shader fshader(fshaderCode, GL_FRAGMENT_SHADER);

    ShaderProgram program(vshader, fshader);
    program.findAttributes({"a_Pos", "a_Color"});

    vshader.cleanup();
    fshader.cleanup();

    cout << "Locations: " << program["a_Pos"] << " " << program["a_Color"] << " " << program["asd"] << endl;

    cout << "ShaderProgram status: " << program.status << endl;

    //set up VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);

    



    //set up drawing
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    //draw loop
    do {
        glClear(GL_COLOR_BUFFER_BIT);


        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwWindowShouldClose(window) != GLFW_TRUE && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);

    //clean up


    glfwTerminate();
    return 0;
}
