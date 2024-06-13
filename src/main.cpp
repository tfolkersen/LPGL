#include <iostream>
#include "glstuff.h"
#include "lpglctx.h"

using namespace std;

int main() {
    LPGLctxptr ctx = LPGLctx::fromParameters(1024, 768, "lpgl-main");

    ctx->cleanup();

    LPGLctx::terminate();
    return 0;
}
