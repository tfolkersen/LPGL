#version 300 es
precision highp float;

uniform mat3 u_Camera;
uniform mat3 u_RotScale;
uniform float u_Border;
uniform vec2 u_NewCenter;

uniform mat3 u_Rot;
uniform mat3 u_Scale;

in vec2 a_Pos;
in float a_Ordinal;

out vec2 v_Pos;


out float mx;
out float my;

out vec2 up;
out vec2 right;

#define SCREEN_X 200
#define SCREEN_Y 200

const vec2 c_ScreenCoords[4] = vec2[4](
    vec2(0.0, SCREEN_Y),
    vec2(SCREEN_X, SCREEN_Y),
    vec2(SCREEN_X, 0.0), 
    vec2(0.0, 0.0)
);

out vec2 v_ScreenPos;

void main() {
    gl_Position = vec4(a_Pos.x, a_Pos.y, 0.0, 1.0);

    float x = ((a_Pos.x + 1.0) / 2.0) * 200.0;
    float y = ((a_Pos.y + 1.0) / 2.0) * 200.0;


    vec3 screenPos = vec3(c_ScreenCoords[int(a_Ordinal)], 1.0);
    screenPos = vec3(x, y, 1.0);

    // SCREEN SPACE
    //screenPos = screenPos;

    // WORLD SPACE
    screenPos = u_Camera * screenPos;

    // LOCAL/DRAWCALL SPACE
    vec3 offset = vec3(u_NewCenter.x, u_NewCenter.y, 0.0);
    vec3 camOffset = vec3(100.0, 100.0, 0.0);
    //screenPos = ((u_Scale * offset) + (inverse(u_Rot) * u_Scale * (u_Camera * screenPos - offset))) - offset;
    //screenPos = u_Camera * screenPos;





    v_ScreenPos = screenPos.xy;


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

