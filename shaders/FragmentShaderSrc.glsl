#version 400 core

in vec3 outColor;
in vec2 texCoord;

out vec4 fragColor;

uniform sampler2D myTexture1;
uniform sampler2D myTexture2;

void main() {
    fragColor =
    mix(texture(myTexture2, texCoord), texture(myTexture2,texCoord), 0.2f) * vec4(outColor, 1.0f);
    /*vec4 tex1 = texture(myTexture1, texCoord),
         tex2 = texture(myTexture2, texCoord);
    if (tex1.x+tex1.y+tex1.z >= 3.0f) {
        fragColor = tex2;
    }else{
        fragColor = tex1;
    }*/
}
