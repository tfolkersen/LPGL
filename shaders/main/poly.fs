#version 300 es
precision highp float;

uniform float u_Border;

uniform vec2 u_Center;
uniform mat3 u_RotScale;

in vec2 v_Pos;

out vec4 o_FragColor;

vec2 p;

uniform vec2 a;
uniform vec2 b;
uniform vec2 c;

bool crossTest();
void texCoords();

in float mx;
in float my;

in vec2 up;
in vec2 right;

vec3 tc;

void main() {
    p = vec2(floor(v_Pos.x), floor(v_Pos.y));

    if (length(p) < 6.0) {
        o_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
        return;
    }

    if (length(p - u_Center) < 6.0) {
        o_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
        return;
    }

    if (crossTest()) {
        texCoords();

        vec4 c1 = vec4(1.0, 0.0, 0.0, 1.0);
        vec4 c2 = vec4(0.0, 1.0, 0.0, 1.0);
        vec4 c3 = vec4(0.0, 0.0, 1.0, 1.0);
        o_FragColor = tc.x * c1 + tc.y * c2 + tc.z * c3;
        //o_FragColor = c1;




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


    int signs[3];
    signs[0] = 0;
    signs[1] = 0;
    signs[2] = 0;


    signs[int(sign(cross1)) + 1] += 1;
    signs[int(sign(cross2)) + 1] += 1;
    signs[int(sign(cross3)) + 1] += 1;

    //if ((sign(cross1) == sign(cross2) && sign(cross2) == sign(cross3)) || (cross1 == 0.0 || cross2 == 0.0 || cross3 == 0.0)) {
    if (!(signs[0] > 0 && signs[2] > 0)) {
        if (u_Border <= 0.0) {
            return true;
        }

        float epsilon = 0.001;

        o_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        vec2 abn = normalize(ab);
        vec2 v1 = ap - dot(abn, ap) * abn;
        float l1 = length(v1);

        if (l1 < epsilon) {
            return true;
        }

        vec2 v1n = v1 / l1;
        float dist1 = length(v1);
        float max1 = mx * abs(dot(v1n, right)) + my * abs(dot(v1n, up));
        if (dist1 < max1) {
            return true;
        }

        o_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        vec2 bcn = normalize(bc);
        vec2 v2 = bp - dot(bcn, bp) * bcn;
        float l2 = length(v2);
        if (l2 < epsilon) {
            return true;
        }
        vec2 v2n = v2 / l2;
        float dist2 = length(v2);
        float max2 = mx * abs(dot(v2n, right)) + my * abs(dot(v2n, up));
        if (dist2 < max2) {
            return true;
        }

        o_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
        vec2 can = normalize(ca);
        vec2 v3 = cp - dot(can, cp) * can;
        float l3 = length(v3);
        if (l3 < epsilon) {
            return true;
        }
        vec2 v3n = v3 / l3;
        float dist3 = length(v3);
        float max3 = mx * abs(dot(v3n, right)) + my * abs(dot(v3n, up));
        if (dist3 < max3) {
            return true;
        }

    }

    return false;
}


void texCoords() {
    mat3 coordMat;

    coordMat[0] = vec3(a, 1.0);
    coordMat[1] = vec3(b, 1.0);
    coordMat[2] = vec3(c, 1.0);

    tc = inverse(coordMat) * vec3(p, 1.0);
}
