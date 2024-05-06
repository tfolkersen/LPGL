#version 330 core

uniform sampler2D u_Tex0;
uniform sampler2D u_Tex1;

in vec2 v_UV;

out vec3 fragColor;

void main() {
    vec2 uv = vec2(v_UV.x, v_UV.y);

    vec4 color0 = texture(u_Tex0, uv);
    vec4 color1 = texture(u_Tex1, uv);

    fragColor = (0.5 * color0 + 0.5 * color1).xyz;
}
