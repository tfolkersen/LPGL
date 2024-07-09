#version 300 es
precision highp float;


uniform mat3 fillpTransform;
uniform mat3 model;
uniform mat3 camera;
uniform vec2 u_Center;

in vec2 a_Pos;
in vec3 a_Color;



out vec3 v_Color;
out vec2 v_p;
out vec2 v_sp;

out float sx;
out float sy;

void main() {
    float x = ((a_Pos.x + 1.0) / 2.0) * 200.0;
    float y = ((a_Pos.y + 1.0) / 2.0) * 200.0;

    vec3 fill3 = vec3(x, y, 1.0);
    // FILLP SCREEN SPACE
    //fill3 = fillpTransform * fill3;
    //v_sp = fill3.xy;

    // FILLP WORLD SPACE
    //fill3 = (fillpTransform * camera * fill3);
    //v_sp = fill3.xy;

    // FILLP DRAWCALL SPACE
    //fill3 = (model * camera * fill3);
    //v_sp = fill3.xy + u_Center;
    fill3 = (fillpTransform * (vec3(u_Center, 0.0) + model * camera * fill3));
    v_sp = fill3.xy;

    vec3 pos = vec3(x, y, 1.0);
    v_p = (model * camera * pos).xy;

    //sx = length((camera * model * vec3(1.0, 0.0, 1.0)).xy);
    //sy = length((camera * model * vec3(0.0, 1.0, 1.0)).xy);

    sx = length((model * camera * vec3(1.0, 0.0, 1.0)).xy);
    sy = length((model * camera * vec3(0.0, 1.0, 1.0)).xy);


    gl_Position = vec4(a_Pos, 0.0, 1.0);
}
