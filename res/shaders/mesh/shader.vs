#version 330 core
#extension GL_ARB_explicit_uniform_location : require
#extension GL_ARB_shading_language_420pack : require

layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec2 uvs;
layout(location = 2) in vec3 normal;
layout(location = 0)uniform mat4 MVP;
layout(location = 1) uniform vec4 pos;
layout(location = 2) uniform vec4 scale;
layout(location = 3) uniform vec4 rotation;
layout(location = 4) uniform vec3 light;
layout(location = 5) uniform vec3 primaryColor;



out float vcol;
out vec2 UV;
out vec3 PrimColor;

void main(){
    vec4 verPos = vec4(vertexPosition,1);
    verPos.x = vertexPosition.x * cos(rotation.z) - vertexPosition.y * sin(rotation.z);
    verPos.y = vertexPosition.x * sin(rotation.z) + vertexPosition.y * cos(rotation.z);
    verPos.z = ;
    verPos = (verPos * scale + pos);
    gl_Position =  MVP * verPos;

    vec3 L = light - verPos.xyz;
    L = L/L.length();
    vec3 norm = normal;
    x = norm.x;
    y = norm.y;
    norm.x = x * cos(rotation.z) - y * sin(rotation.z);
    norm.y = x * sin(rotation.z) + y * cos(rotation.z);
    vcol = (length(dot(norm,L))/length(norm)/length(L));

    UV = uvs;
    UV;
    PrimColor = primaryColor;
}