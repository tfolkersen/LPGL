#include <cstdio>
#include <cstdlib>
#include <string.h>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>


#include <iostream>

#include <string>
#include <vector>
#include <unordered_map>


#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <stb_image.h>

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


//////////////////////////////////////////////////////////// TEXTURES
enum TEXTURECONSTANTS {
    TEXTURE_ERROR = -1,
    TEXTURE_DELETED = 0,
    TEXTURE_OK = 1,
    TEXTURE_FILENAME,
    TEXTURE_RAWDATA,
};

struct Texture {
    GLuint id;
    int status;
    std::string statusLog;

    Texture(const void *buffer, int bufferType, int width = 0, int height = 0, int channels = 0);
    void cleanup();
};



Texture::Texture(const void *buffer, int bufferType, int width, int height, int channels) {
    unsigned char *stbi_data = NULL;

    if (bufferType == TEXTURE_FILENAME) {
        stbi_data = stbi_load((const char *) buffer, &width, &height, &channels, 0);

        if (!stbi_data) {
            id = 0;
            status = TEXTURE_ERROR;
            statusLog = "Texture failed stbi_load of file: " + string((const char *) buffer);
            
            cerr << statusLog << endl;
            return;
        }
    } else if (bufferType == TEXTURE_RAWDATA) {
        //nothing to do here?
    } else {
        id = 0;
        status = TEXTURE_ERROR;
        statusLog = "Texture constructor invalid bufferType: " + to_string(bufferType);

        cerr << statusLog << endl;
        return;
    }

    //Make texture
    glGenTextures(1, &id);

    if (id == 0) {
        status = TEXTURE_ERROR;
        statusLog = "Texture failed call to glGenTextures";

        cerr << statusLog << endl;

        if (stbi_data) {
            stbi_image_free(stbi_data);
        }

        return;
    }

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    const void *imgBuffer = bufferType == TEXTURE_FILENAME ? stbi_data : buffer;


    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, imgBuffer);

    status = TEXTURE_OK;
    statusLog = "Success";

    if (stbi_data) {
        stbi_image_free(stbi_data);
    }
}

void Texture::cleanup() {
    if (status == TEXTURE_OK) {
        glDeleteTextures(1, &id);
    }

    id = 0;
    status = TEXTURE_DELETED;
}


////////////////////////////////////////////////////////////


GLFWwindow *window = NULL;


GLint u_Model, u_View, u_Projection;



template <class T>
T clamp(const T &x, const T &low, const T &high) {
    if (x < low) {
        return low;
    }

    if (x > high) {
        return high;
    }

    return x;
}

uint64_t frameNumber = 0;

GLfloat boxX = 0.0f;
GLfloat boxSpeed = 0.01f;
GLfloat direction = 1.0f;


glm::mat4 matProjection = glm::perspective(glm::radians(45.0f), 4.0f / 3.0f, 0.1f, 100.0f);

GLfloat angLeft = 0.0f;
GLfloat angUp = 0.0f;
glm::vec3 pos(0.0f, 0.0f, 5.0f);
glm::vec4 motion4(0.0f, 0.0f, 0.0f, 1.0f);
GLfloat moveSpeed = 0.05f;

void handleKeys() {
    float deltaAng = 0.01;
    motion4 = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        angUp += deltaAng;
    }

    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        angUp -= deltaAng;
    }

    angUp = clamp(angUp, glm::radians(-89.0f), glm::radians(89.0f));

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        angLeft += deltaAng;
    }

    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        angLeft -= deltaAng;
    }

    glm::vec3 motion(0.0f, 0.0f, 0.0f);


    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        motion.z -= 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        motion.z += 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        motion.x -= 1.0f;
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        motion.x += 1.0f;
    }

    if (!(0.0f == motion.x && motion.x == motion.y && motion.y == motion.z)) {
        motion = glm::normalize(motion);
    }

    motion4 = glm::vec4(motion.x, motion.y, motion.z, 1.0f);
}


const glm::mat4 identity = glm::identity<glm::mat4>();

void animate() {
    handleKeys();

    //Set up uniforms
    glm::mat4 matModel = identity;
    glm::mat4 matView = identity;

    matModel = glm::translate(matModel, {0, 0, 0});


    glm::vec4 forward(0.0f, 0.0f, -1.0f, 1.0f);
    glm::mat4 rotUp = glm::rotate(glm::identity<glm::mat4>(), angUp, glm::vec3(1.0f, 0.0f, 0.0f));
    glm::mat4 rotLeft = glm::rotate(glm::identity<glm::mat4>(), angLeft, glm::vec3(0.0f, 1.0f, 0.0f));

    forward = rotLeft * rotUp * forward; 

    motion4 = rotLeft * rotUp * motion4;


    glm::vec3 truncForward = glm::vec3(forward.x, forward.y, forward.z);
    glm::vec3 truncMotion = glm::vec3(motion4.x, motion4.y, motion4.z);

    pos += truncMotion * moveSpeed;

    matView = glm::lookAt(pos, pos + truncForward, glm::vec3(0.0f, 1.0f, 0.0f));

    glUniformMatrix4fv(u_Model, 1, GL_FALSE, &matModel[0][0]);
    glUniformMatrix4fv(u_View, 1, GL_FALSE, &matView[0][0]);
    glUniformMatrix4fv(u_Projection, 1, GL_FALSE, &matProjection[0][0]);

    GLfloat delta = boxSpeed * direction;
    boxX = clamp(boxX + delta, -0.9f, 0.9f);
    if (abs(boxX) >= 0.9f) {
        direction *= -1.0f;
    }
}


static void cursorPosCallback(GLFWwindow *win, double xPos, double yPos) {
    cout << xPos << " " << yPos << endl;

}



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
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);




    //init GLEW
    if (glewInit() != GLEW_OK) {
        cerr << "Failed to init GLEW" << endl;
        glfwTerminate();
        return -1;
    }

    //buffer data
    GLfloat vertexData[] = {
        -0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, //7 red
        0.5f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f, //9 blue
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, //3 green
        -0.5f, -0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, //1 yellow
    };

    GLint elementData[] = {
        0, 1, 2,
        2, 3, 0,
    };

    Texture texture("cubetest.png", TEXTURE_FILENAME);

    GLuint vbuffer;
    glGenBuffers(1, &vbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexData), vertexData, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    GLuint ebuffer;
    glGenBuffers(1, &ebuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(elementData), elementData, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    //set up shader program
    string vshaderCode = readFile("shaders/cubetest.vshader");
    string fshaderCode = readFile("shaders/cubetest.fshader");

    Shader vshader(vshaderCode, GL_VERTEX_SHADER);
    Shader fshader(fshaderCode, GL_FRAGMENT_SHADER);

    ShaderProgram program(vshader, fshader);
    program.findAttributes({"a_Pos", "a_Color", "a_UV"});
    program.findUniforms({"u_Model", "u_View", "u_Projection", "u_Tex0"});

    u_Model = program["u_Model"];
    u_View = program["u_View"];
    u_Projection = program["u_Projection"];



    vshader.cleanup();
    fshader.cleanup();

    cout << "Locations: " << program["a_Pos"] << " " << program["a_Color"] << " " << program["asd"] << endl;

    cout << "ShaderProgram status: " << program.status << endl;

    //set up VAO
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebuffer);

    GLint a_Pos = program["a_Pos"];
    GLint a_Color = program["a_Color"];
    GLint a_UV = program["a_UV"];

    glEnableVertexAttribArray(a_Pos);
    glVertexAttribPointer(a_Pos, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), 0);

    glEnableVertexAttribArray(a_Color);
    glVertexAttribPointer(a_Color, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *) (3 * sizeof(GLfloat)));

    glEnableVertexAttribArray(a_UV);
    glVertexAttribPointer(a_UV, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(GLfloat), (void *) (6 * sizeof(GLfloat)));



    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glDisableVertexAttribArray(a_Pos);
    glDisableVertexAttribArray(a_Color);

    //set up drawing
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glUseProgram(program.id);
    glBindVertexArray(vao);



    GLint u_Tex0 = program["u_Tex0"];

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.id);

    glUniform1i(u_Tex0, GL_TEXTURE0);

    //draw loop
    do {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        animate();
    } while (glfwWindowShouldClose(window) != GLFW_TRUE && glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS);

    //clean up


    glfwTerminate();
    return 0;
}
