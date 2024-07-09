#version 300 es
precision highp float;

uniform vec2 a;
uniform vec2 b;
uniform vec2 c;

uniform float u_Border;
uniform vec2 u_Center;
uniform int u_DrawCirc;
uniform int u_Fillp[2];

in float sx;
in float sy;

in vec3 v_Color;

in vec2 v_p;
vec2 p;

in vec2 v_sp;
int screenX, screenY;

out vec4 o_FragColor;


bool crossTest();
void texCoords();
vec3 tc;

void main() {
    o_FragColor = vec4(0.0, 0.0, 0.0, 0.0);

    float snapX = 1.0 / sx;
    float fracX = v_p.x - floor(v_p.x);
    float keepX = floor(fracX / snapX) * snapX;
    p.x = floor(v_p.x) + keepX;

    float snapY = 1.0 / sy;
    float fracY = v_p.y - floor(v_p.y);
    float keepY = floor(fracY / snapY) * snapY;
    p.y = floor(v_p.y) + keepY;


    screenX = int(floor(v_sp.x));
    screenY = int(floor(v_sp.y));

    int fillBytePos = screenY % 8;
    int fillByteOrdinal = fillBytePos <= 3 ? 1 : 0;
    int fillByte = 0xff & (u_Fillp[fillByteOrdinal] >> (8 * (fillBytePos % 4)));
    int fillBit = 0x1 & (fillByte >> (7 - screenX % 8));


    if (fillBit == 1) {
        o_FragColor = vec4(1.0, 0.0, 1.0, 1.0);
        return;
    }



    if (u_DrawCirc == 1) {
        if (length(p - a) <= 3.0) {
            o_FragColor = vec4(1.0, 1.0, 0.0, 1.0);
        }
        return;
    }


    if (length(p - u_Center) <= 3.0) {
        o_FragColor = vec4(1.0, 0.5, 0.0, 1.0);
        return;
    }

    if (crossTest()) {
        texCoords();
    
        vec4 c1 = vec4(1.0, 0.0, 0.0, 1.0);
        vec4 c2 = vec4(0.0, 1.0, 0.0, 1.0);
        vec4 c3 = vec4(0.0, 0.0, 1.0, 1.0);

        o_FragColor = tc.x * c1 + tc.y * c2 + tc.z * c3;

        return;
    } else {
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

    vec4 keepColor = o_FragColor;

    if (!(signs[0] > 0 && signs[2] > 0)) {
        //return true;
        float epsilon = 0.001;

        vec2 realRight = vec2(1.0, 0.0);
        vec2 realUp = vec2(0.0, 1.0);

        o_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
        vec2 abn = normalize(ab);
        vec2 v1 = ap - dot(abn, ap) * abn;
        float dist1 = length(v1);

        if (dist1 < epsilon) {
            return true;
        }

        if (dist1 < u_Border) {
            return true;
        }

        o_FragColor = vec4(0.0, 1.0, 0.0, 1.0);
        vec2 bcn = normalize(bc);
        vec2 v2 = bp - dot(bcn, bp) * bcn;
        float dist2 = length(v2);

        if (dist2 < epsilon) {

            return true;
        }

        if (dist2 < u_Border) {
            return true;
        }

        o_FragColor = vec4(0.0, 0.0, 1.0, 1.0);
        vec2 can = normalize(ca);
        vec2 v3 = cp - dot(can, cp) * can;
        float dist3 = length(v3);

        if (dist3 < epsilon) {
            return true;
        }

        if (dist3 < u_Border) {
            return true;
        }

    }

    o_FragColor = keepColor;
    return false;
}

void texCoords() {
    mat3 coordMat;

    coordMat[0] = vec3(a, 1.0);
    coordMat[1] = vec3(b, 1.0);
    coordMat[2] = vec3(c, 1.0);

    tc = inverse(coordMat) * vec3(p, 1.0);
}

