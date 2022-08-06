#version 330 core

in float vcol;
in vec2 UV;
in vec3 PrimColor;

out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
    vec3 ocol = texture( myTextureSampler, UV ).rgb;
    if(ocol.y > max(ocol.x, ocol.z)){
        ocol = PrimColor;
    }
    ocol = ocol * vcol;
    color = vec4(ocol,1);
}