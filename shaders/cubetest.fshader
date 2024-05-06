#version 330 core

in vec3 v_Color;
in vec2 v_UV;

uniform sampler2D u_Tex0;

out vec3 fragColor;

void main() {
    vec2 fixedUV = vec2(v_UV.x, 1.0 - v_UV.y);

    fragColor = texture(u_Tex0, fixedUV).xyz;
}
