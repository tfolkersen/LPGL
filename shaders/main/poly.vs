#version 300 es
precision highp float;

uniform mat4 u_Mat1;
uniform vec2 u_Center;

uniform mat4 u_RotScale;

in vec2 a_Pos;

out vec2 v_Pos;

void main() {
    gl_Position = vec4(a_Pos.x, a_Pos.y, 0.0, 1.0);

    float x = ((a_Pos.x + 1.0) / 2.0) * 200.0;
    float y = ((a_Pos.y + 1.0) / 2.0) * 200.0;

    vec4 pos = vec4(x, y, 0.0, 1.0);


    pos -= vec4(u_Center, 0.0, 0.0);
    pos = u_RotScale * pos;
    pos += vec4(u_Center, 0.0, 0.0);
    


    v_Pos = pos.xy;
}


