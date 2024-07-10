#version 300 es
precision highp float;


in vec2 a_Pos;

out vec2 v_Pos;

void main() {
    gl_Position = vec4(a_Pos, 0.0, 1.0);
    v_Pos = a_Pos;
}
