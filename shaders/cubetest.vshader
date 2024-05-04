#version 330 core

in vec3 a_Pos;
in vec3 a_Color;

out vec3 color;

void main() {
    gl_Position = vec4(a_Pos, 1.0);
    color = a_Color;
}
