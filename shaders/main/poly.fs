#version 300 es
precision highp float;


uniform float u_Tri[6];
uniform float u_Border;

uniform float u_ScaleX;
uniform float u_ScaleY;

uniform vec2 u_Center;
uniform mat4 u_RotScale;

in vec2 v_Pos;

out vec4 o_FragColor;

vec2 a, b, c, p;

bool crossTest();


float mx;
float my;

vec2 up;
vec2 right;

void main() {
    a = vec2(u_Tri[0], u_Tri[1]);
    b = vec2(u_Tri[2], u_Tri[3]);
    c = vec2(u_Tri[4], u_Tri[5]);
    p = vec2(floor(v_Pos.x), floor(v_Pos.y));

    a = (u_RotScale * (vec4(a - u_Center, 0.0, 1.0))).xy + u_Center;
    b = (u_RotScale * (vec4(b - u_Center, 0.0, 1.0))).xy + u_Center;
    c = (u_RotScale * (vec4(c - u_Center, 0.0, 1.0))).xy + u_Center;

    up = normalize((u_RotScale * vec4(0.0, 1.0, 0.0, 1.0)).xyz).xy;
    right = normalize((u_RotScale * vec4(1.0, 0.0, 0.0, 1.0)).xyz).xy;

    mx = length((u_RotScale * vec4(1.0, 0.0, 0.0, 1.0)).xyz) * u_Border;
    my = length((u_RotScale * vec4(0.0, 1.0, 0.0, 1.0)).xyz) * u_Border;

    if (length(p) < 6.0) {
        o_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
        return;
    }

    if (length(p - u_Center) < 6.0) {
        o_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
        return;
    }

    if (crossTest()) {
        //o_FragColor = vec4(v_Pos.x / 200.0, 0.0, v_Pos.y / 200.0, 1.0);
    } else {
        discard;
    }
}


bool crossTest() {
    vec2 ab = b - a;
    vec2 bc = c - b;
    vec2 ca = a - c;

    vec2 ap = p - a;
    vec2 bp = p - b;
    vec2 cp = p - c;

    float cross1 = ab.x * ap.y - ab.y * ap.x;
    float cross2 = bc.x * bp.y - bc.y * bp.x;
    float cross3 = ca.x * cp.y - ca.y * cp.x;


    if (sign(cross1) == sign(cross2) && sign(cross2) == sign(cross3)) {
        if (u_Border <= 0.0) {
            return true;
        }

        float epsilon = 0.001;

        vec2 abn = normalize(ab);
        vec2 v1 = ap - dot(abn, ap) * abn;
        vec2 v1n = normalize(v1);
        float dist1 = length(v1);
        float max1 = mx * abs(dot(v1n, right)) + my * abs(dot(v1n, up));
        if (dist1 <= max1 + epsilon) {
            o_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
            return true;
        }

        vec2 bcn = normalize(bc);
        vec2 v2 = bp - dot(bcn, bp) * bcn;
        vec2 v2n = normalize(v2);
        float dist2 = length(v2);
        float max2 = mx * abs(dot(v2n, right)) + my * abs(dot(v2n, up));
        if (dist2 <= max2 + epsilon) {
            o_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
            return true;
        }

        vec2 can = normalize(ca);
        vec2 v3 = cp - dot(can, cp) * can;
        vec2 v3n = normalize(v3);
        float dist3 = length(v3);
        float max3 = mx * abs(dot(v3n, right)) + my * abs(dot(v3n, up));
        if (dist3 <= max3 + epsilon) {
            o_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
            return true;
        }

    }

    return false;
}

