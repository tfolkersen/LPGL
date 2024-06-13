#version 300 es

precision highp float;


in vec2 v_Pos;
in vec3 v_Color;

out vec4 fragColor;

uniform float u_ax;
uniform float u_ay;
uniform float u_bx;
uniform float u_by;
uniform float u_cx;
uniform float u_cy;

/*
    a x b

        i  j  k
        ax ay 0
        bx by 0

        i(0) - j(0) + k(ax * by - ay * bx)

*/


void main() {
    fragColor = vec4(1.0, 1.0, 1.0, 0.0);

    float x = ((v_Pos.x + 1.0) / 2.0) * 1024.0;
    float y = ((v_Pos.y + 1.0) / 2.0) * 768.0;

    vec2 a = vec2(u_ax, u_ay);
    vec2 b = vec2(u_bx, u_by);
    vec2 c = vec2(u_cx, u_cy);

    vec2 p = vec2(x, y);

    vec2 ab = b - a;
    vec2 bc = c - b;
    vec2 ca = a - c;

    vec2 ap = p - a;
    vec2 bp = p - b;
    vec2 cp = p - c;

    float cross1 = ab.x * ap.y - ab.y * ap.x;
    float cross2 = bc.x * bp.y - bc.y * bp.x;
    float cross3 = ca.x * cp.y - ca.y * cp.x;

    fragColor = vec4(0.0, 0.0, 0.0, 0.0);


    if (sign(cross1) == sign(cross2) && sign(cross2) == sign(cross3)) {

        // compute distances
        fragColor.a = 1.0;

        vec2 abn = normalize(ab);
        vec2 bcn = normalize(bc);
        vec2 can = normalize(ca);

        float dist1 = length(ap - dot(abn, ap) * abn);
        float dist2 = length(bp - dot(bcn, bp) * bcn);
        float dist3 = length(cp - dot(can, cp) * can);

        float maxDist = 4.0;

        bool found = false;

        if (dist1 < maxDist) {
            fragColor.x = 1.0;
            found = true;
        }

        if (dist2 < maxDist) {
            fragColor.y = 1.0;
            found = true;
        }

        if (dist3 < maxDist) {
            fragColor.z = 1.0;
            found = true;
        }

        if (!found) {
            fragColor.xyz = vec3(1.0, 1.0, 1.0);
        }

    }



}
