#version 330 core

in vec3 a_Pos;
in vec2 a_UV;

out vec2 v_UV;

void main() {
    gl_Position = vec4(a_Pos.xyz, 1.0);
    v_UV = a_UV;
}
