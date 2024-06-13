#version 300 es
precision lowp float;

in vec2 v_UV;

uniform float u_Offset;
uniform sampler2D u_Tex0;

out vec3 fragColor;

void main() {
    vec2 uv = vec2(v_UV.x + u_Offset, v_UV.y);

    vec3 color1 = texture(u_Tex0, uv).xyz;

    fragColor = color1;
}
