#include <iostream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/dummy.hpp>

using namespace std;

int main() {
    cout << "GLEW version: " << glewGetString(GLEW_VERSION) << endl;

    {
        int major, minor, revision;
        glfwGetVersion(&major, &minor, &revision);

        cout << "GLFW version: " << major << "." << minor << "." << revision << endl;
    }

    return 0;
}
