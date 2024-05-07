#pragma once

#include <GL/glew.h>
#include <string>

enum TEXTURE_CONSTANTS {
    TEXTURE_ERROR = -1,
    TEXTURE_EMPTY = 0,
    TEXTURE_OK = 1,
};

struct Texture {
    GLuint id;
    int status;
    std::string statusLog;

    static Texture fromFile(const char *fileName);

    //Texture(const void *buffer, int bufferType, int width = 0, int height = 0, int channels = 0);
    void cleanup();

  private:
    //Texture();
};





