#version 400 core
layout(location=1) in vec3 pos;
layout(location=2) in vec3 aColor;
layout(location=3) in vec2 aTexCoord;

out vec3 outColor;
out vec2 texCoord;

uniform mat4 MVP;

void main() {
    gl_Position = MVP * vec4(pos.xyz, 1.0f);
    outColor = aColor;
    texCoord = aTexCoord;
}