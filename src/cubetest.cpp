#include <cstdio>
#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

using namespace std;

GLFWwindow *window = NULL;

string readFile(const string &fileName) {
    string strData;

    FILE *f = fopen(fileName.c_str(), "r");

    if (f == NULL) {
        cerr << "readFile failed to read " << fileName << endl;
        return "";
    }

    fseek(f, 0, SEEK_END);
    long size = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *data = new char[size + 1];
    data[size] = 0;

    fread(data, 1, size, f);

    strData = data;

    delete[] data;
    fclose(f);
    return strData;
}

int main() {
    //init GLFW
    if (glfwInit() != GLFW_TRUE) {
        cerr << "Failed to init GLFW" << endl;
        return -1;
    }

    //make window
    glfwWindowHint(GLFW_SAMPLES, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);

    window = glfwCreateWindow(1024, 768, "cubetest", NULL, NULL);

    glfwMakeContextCurrent(window);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GLFW_TRUE);

    //init GLEW
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to init GLEW" << endl;
        glfwTerminate();
        return -1;
    }

    //compile shaders
    GLint vsCompileStatus;

    GLuint vshader = glCreateShader(GL_VERTEX_SHADER);

    string vshaderSource = readFile("shaders/cubetest.vshader");
    const char *vshaderStrPtr = vshaderSource.c_str();

    glShaderSource(vshader, 1, &vshaderStrPtr, NULL);
    glCompileShader(vshader);

    glGetShaderiv(vshader, GL_COMPILE_STATUS, &vsCompileStatus);

    if (vsCompileStatus == GL_TRUE) {
        cerr << "Compiled vshader" << endl;
    } else {
        cerr << "Failed to compile vshader:" << endl;

        int infoLogLength;
        glGetShaderiv(vshader, GL_INFO_LOG_LENGTH, &infoLogLength);

        char *log = new char[infoLogLength];

        glGetShaderInfoLog(vshader, infoLogLength, NULL, log);

        cerr << log << endl;

        delete[] log;
    }

    GLint fsCompileStatus;

    GLuint fshader = glCreateShader(GL_FRAGMENT_SHADER);

    string fshaderSource = readFile("shaders/cubetest.fshader");
    const char *fshaderStrPtr = fshaderSource.c_str();

    glShaderSource(fshader, 1, &fshaderStrPtr, NULL);
    glCompileShader(fshader);

    glGetShaderiv(fshader, GL_COMPILE_STATUS, &fsCompileStatus);

    if (fsCompileStatus == GL_TRUE) {
        cerr << "Compiled fshader" << endl;
    } else {
        cerr << "Failed to compile fshader:" << endl;

        GLint infoLogLength;
        glGetShaderiv(fshader, GL_INFO_LOG_LENGTH, &infoLogLength);

        char *log = new char[infoLogLength];

        glGetShaderInfoLog(fshader, infoLogLength, NULL, log);

        cerr << log << endl;

        delete[] log;
    }

    //create and link shader program, clean up
    GLuint program = glCreateProgram();
    glAttachShader(program, vshader);
    glAttachShader(program, fshader);
    glLinkProgram(program);

    GLint linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);

    if (linkStatus == GL_TRUE) {
        cerr << "Program linked successfully" << endl;
    } else {
        cerr << "Failed to link program:" << endl;

        GLint infoLogLength;

        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &infoLogLength);

        char *log = new char[infoLogLength];

        glGetProgramInfoLog(program, infoLogLength, NULL, log);

        cout << log << endl;

        delete[] log;
    }

    glDeleteShader(vshader);
    glDeleteShader(fshader);

    //buffer data
    GLfloat vertexData[] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 1.0f,
        0.0f, 0.5f, 0.0f, 1.0f, 0.0f, 1.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f,
    };

    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);

    //Enable attributes
    int a_Pos = glGetAttribLocation(program, "a_Pos");

    if (a_Pos == -1) {
        cerr << "Failed to find a_Pos location" << endl;
    }

    int a_Color = glGetAttribLocation(program, "a_Color");

    if (a_Pos == -1) {
        cerr << "Failed to find a_Color location" << endl;
    }


    glEnableVertexAttribArray(a_Pos);
    glVertexAttribPointer(a_Pos, 3, GL_FLOAT, false, 6 * sizeof(GLfloat), 0);

    glEnableVertexAttribArray(a_Color);
    glVertexAttribPointer(a_Color, 3, GL_FLOAT, false, 6 * sizeof(GLfloat), (int *) (3 * sizeof(GLfloat)));



    //set up drawing
    glClearColor(0.3f, 0.15f, 0.0f, 1.0f);
    glUseProgram(program);

    //draw loop
    do {
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    } while (glfwWindowShouldClose(window) != GLFW_TRUE && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);




    //clean up
    glfwTerminate();
    return 0;
}
