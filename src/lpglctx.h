#pragma once

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <memory>
#include <string>
#include <vector>

#include "glstuff.h"

enum LPGLctxEnum {
    LPGLCTX_ERROR = -1,
    LPGLCTX_EMPTY = 0,
    LPGLCTX_OK = 1,
};

struct LPGLctx {
    GLFWwindow *window;
    LPGLctxEnum status;
    std::string statusLog;

    static std::unique_ptr<LPGLctx> fromParameters(const int w, const int h, const char *windowTitle);
    static void terminate();

  private:
    LPGLctx();

  public:
    ~LPGLctx();

    LPGLctx(const LPGLctx &other) = delete;
    LPGLctx &operator=(const LPGLctx &other) = delete;

    /*
        TODO probably don't need move semantics either because the static
        constructor function returns a unique_ptr
    */
    LPGLctx(LPGLctx &&other) noexcept;
    LPGLctx &operator=(LPGLctx &&other) noexcept;

    void release();
    void cleanup();

    friend std::ostream &operator<<(std::ostream &os, const LPGLctx &ctx);
    friend std::ostream &operator<<(std::ostream &os, LPGLctx &ctx);

    void buildFromParameters(const int w, const int h, const char *windowTitle, bool _doCleanup = true);

    void makeCurrent();

  private:
    void moveFrom(LPGLctx &&other, bool _doCleanup = true);

    void initGLData();
    void freeGLData();

    ////////////////////////////// Private data
    // Buffers
    GLuint fullBox_vbuff;
    GLuint boxLike_ebuff;

    // VAOs
    GLuint tri_vao;

    // Programs
    GLprogram tri_pr;

  public:
    void drawTri(const std::vector<GLfloat> &coords);


};

typedef std::unique_ptr<LPGLctx> LPGLctxptr;
