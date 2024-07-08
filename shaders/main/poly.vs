#version 300 es
precision highp float;

uniform mat3 u_Camera;
uniform mat3 u_RotScale;
uniform float u_Border;

in vec2 a_Pos;

out vec2 v_Pos;


out float mx;
out float my;

out vec2 up;
out vec2 right;

void main() {
    gl_Position = vec4(a_Pos.x, a_Pos.y, 0.0, 1.0);

    float x = ((a_Pos.x + 1.0) / 2.0) * 200.0;
    float y = ((a_Pos.y + 1.0) / 2.0) * 200.0;

    vec3 pos = vec3(x, y, 1.0);


    pos = u_Camera * pos;
    
    v_Pos = pos.xy;

    up = (u_RotScale * vec3(0.0, 1.0, 1.0)).xy;
    right = (u_RotScale * vec3(1.0, 0.0, 1.0)).xy;

    float upLen = length(up);
    float rightLen = length(right);

    if (upLen != 0.0) {
        up /= upLen;
    }

    if (rightLen != 0.0) {
        right /= rightLen;
    }

    mx = rightLen * u_Border;
    my = upLen * u_Border;
}

