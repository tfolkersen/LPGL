#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glshader.h"

using namespace std;


int main() {
    cout << "HELLO FROM MAIN.CPP" << endl;

    GLshader<VSHADER> x = GLshader<VSHADER>::fromFile("shaders/cubetest.vshader");
    cout << x << endl;


    cout << "GOODBYE FROM MAIN.CPP" << endl;

    return 0;
}
