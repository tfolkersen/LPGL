#version 300 es
precision highp float;

in vec3 a_Pos;

uniform mat4 model;


void main() {
    vec4 pos = vec4(a_Pos, 1.0);

    pos = model * pos;

    float x = 2.0 * (pos.x / 199.0) - 1.0;
    float y = 2.0 * (pos.y / 199.0) - 1.0;

    gl_Position = vec4(x, y, 0.0, 1.0);
}
