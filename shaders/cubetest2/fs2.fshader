#version 300 es
precision lowp float;

in vec2 v_UV;

uniform sampler2D u_Tex0;
uniform sampler2D u_Tex1;

out vec3 fragColor;

void main() {
    vec2 uv = vec2(v_UV.x, v_UV.y);

    vec3 color0 = texture(u_Tex0, uv).xyz;
    vec3 color1 = texture(u_Tex1, uv).xyz;

    fragColor = 0.5 * (color0 + color1);
}
