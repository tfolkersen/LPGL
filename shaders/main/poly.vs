#version 300 es
precision highp float;

in vec2 a_Pos;

out vec2 v_Pos;

void main() {
    gl_Position = vec4(a_Pos.x, a_Pos.y, 0.0, 1.0);

    float x = ((a_Pos.x + 1.0) / 2.0) * 200.0;
    float y = ((a_Pos.y + 1.0) / 2.0) * 200.0;
    v_Pos = vec2(x, y);
}

