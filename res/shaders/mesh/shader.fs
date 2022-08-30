#version 330 core

in float vCol;
in vec2 UV;
in vec3 PrimColor;

out vec4 color;

uniform sampler2D myTextureSampler;

void main(){
    vec3 objCol = texture( myTextureSampler, UV ).rgb;
    if(objCol.y > max(objCol.x, objCol.z)){
        objCol = PrimColor;
    }
    objCol = objCol * vCol;
    color = vec4(objCol,1.0);
}