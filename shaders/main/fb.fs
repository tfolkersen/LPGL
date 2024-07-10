#version 300 es
precision highp float;

uniform float width;
uniform float height;

uniform sampler2D tex0;

in vec2 v_Pos;

out vec4 fragColor;

void main() {
    vec2 uv;

    uv.x = (v_Pos.x + 1.0) / 2.0;
    uv.y = (v_Pos.y + 1.0) / 2.0;

    fragColor = texture2D(tex0, uv);


}
