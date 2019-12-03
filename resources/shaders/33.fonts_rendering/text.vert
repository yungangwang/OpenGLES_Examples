#version 310 es
layout(location = 0) in vec4 vPos;

out vec2 Texcoord;

uniform mat4 proj;

void main()
{
    gl_Position = proj * vec4(vPos.xy, 0.0, 1.0);
    Texcoord = vPos.zw;
}