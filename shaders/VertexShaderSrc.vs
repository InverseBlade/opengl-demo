#version 400 core
layout(location=1) in vec3 pos;
layout(location=2) in vec3 aColor;

out vec3 fragColor;

uniform float xangle;
uniform float yangle;
uniform float zangle;

void main() {
    float x0, y0, z0;
    vec3 ans = pos;

    x0 = ans.x;
    y0 = ans.y;
    ans.x = x0*cos(zangle) - y0*sin(zangle);
    ans.y = x0*sin(zangle) + y0*cos(zangle);

    x0 = ans.x;
    z0 = ans.z;
    ans.x = x0*cos(yangle) - z0*sin(yangle);
    ans.z = x0*sin(yangle) + z0*cos(yangle);

    z0 = ans.z;
    y0 = ans.y;
    ans.z = z0*cos(xangle) - y0*sin(xangle);
    ans.y = z0*sin(xangle) + y0*cos(xangle);

    gl_Position = vec4(ans.xyz, 1.0f);

    fragColor = vec3(aColor);
}