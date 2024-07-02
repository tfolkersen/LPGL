#version 300 es
precision highp float;


uniform float u_Tri[6];
uniform float u_Border;

in vec2 v_Pos;

out vec4 o_FragColor;

vec2 a, b, c, p;

bool crossTest();


void main() {
    a = vec2(u_Tri[0], u_Tri[1]);
    b = vec2(u_Tri[2], u_Tri[3]);
    c = vec2(u_Tri[4], u_Tri[5]);
    p = vec2(floor(v_Pos.x), floor(v_Pos.y));


    if (crossTest()) {
        o_FragColor = vec4(v_Pos.x / 200.0, 0.0, v_Pos.y / 200.0, 1.0);
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
        float dist1 = length(ap - dot(abn, ap) * abn);
        if (dist1 <= u_Border + epsilon) {
            return true;
        }

        vec2 bcn = normalize(bc);
        float dist2 = length(bp - dot(bcn, bp) * bcn);
        if (dist2 <= u_Border + epsilon) {
            return true;
        }

        vec2 can = normalize(ca);
        float dist3 = length(cp - dot(can, cp) * can);
        if (dist3 <= u_Border + epsilon) {
            return true;
        }

    }

    return false;
}

