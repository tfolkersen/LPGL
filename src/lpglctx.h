#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <memory>
#include <string>
#include <vector>
#include <unordered_set>
#include <glm/glm.hpp>

#include "glstuff.h"

enum LPGLctxEnum {
    LPGLCTX_ERROR = -1,
    LPGLCTX_EMPTY = 0,
    LPGLCTX_OK = 1,
};

/*
    TODO: LPGLctx should inherit from a GLctx struct so that its members are freed on deconstruction, before the GLctx is freed
*/


struct GLctx {
    GLFWwindow *window;

    GLctx();
    ~GLctx();

    // No copying
    GLctx(const GLctx &other) = delete;
    GLctx &operator=(const GLctx &other) = delete;

    // No moving
    GLctx(GLctx &&other) = delete;
    GLctx &operator=(GLctx &&other) = delete;
};


struct LPGLctx: public GLctx {
    LPGLctxEnum status;
    std::string statusLog;

  private:
    // Buffers
    GLuint fullBox_vbuff;
    GLuint boxLike_ebuff;
    std::unordered_set<GLuint> glBuffers;
    std::unordered_set<GLuint> glVertexArrays;
    
    // VAOs
    GLuint tri_vao;
    GLuint poly_vao;

    // Programs
    GLprogram tri_pr;
    GLprogram poly_pr;

    static bool exitHandled;

  private:
    LPGLctx();

  public:
    static std::unique_ptr<LPGLctx> fromParameters(const int w, const int h, const char *windowTitle);
    static void terminate();

    ~LPGLctx();

    // No copying
    LPGLctx(const LPGLctx &other) = delete;
    LPGLctx &operator=(const LPGLctx &other) = delete;

    // No moving
    LPGLctx(LPGLctx &&other) = delete;
    LPGLctx &operator=(LPGLctx &&other) = delete;

  public:
    friend std::ostream &operator<<(std::ostream &os, const LPGLctx &ctx);
    friend std::ostream &operator<<(std::ostream &os, LPGLctx &ctx);

    void makeCurrent();

    GLuint newGlBuffer();
    GLuint newGlVertexArray();

    bool deleteGlBuffer(GLuint buff);
    bool deleteGlVertexArray(GLuint vao);

  private:
    void cleanup();
    void buildFromParameters(const int w, const int h, const char *windowTitle);

    void moveFrom(LPGLctx &&other, bool _doCleanup = true);

    void initGLData();
    void freeGLData();


  public:
    void drawTri(const std::vector<GLfloat> &coords);
    //void drawPoly(const std::vector<GLfloat> &coords);



    glm::mat4 cameraMatrix;
    void setCamera(GLfloat x = 0.0f, GLfloat y = 0.0f, GLfloat angle = 0.0f, GLfloat sx = 1.0f, GLfloat sy = 1.0f);

    void drawPoly(const std::vector<GLfloat> &coords, float angle, float scalex, float scaley);


};

typedef std::unique_ptr<LPGLctx> LPGLctxptr;
