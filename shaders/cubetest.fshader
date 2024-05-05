#version 330 core

in vec3 v_Color;
in vec2 v_UV;

out vec3 fragColor;

uniform sampler2D u_Tex;


void main() {
    vec2 fixedUV = vec2(v_UV.x, 1.0 - v_UV.y);

    vec3 sampleColor = texture2D(u_Tex, fixedUV).xyz;

    //fragColor = vec3(v_UV.x, 0.0f, v_UV.y);
    fragColor = sampleColor;
}
