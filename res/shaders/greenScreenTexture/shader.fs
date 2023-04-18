in vec3 fPosition;
in vec3 fNormal;
in vec3 fLight;
in vec4 fColor;
in vec2 fUV;

out vec4 Color;

uniform sampler2D myTextureSampler;

void main()
{
    vec3 light = normalize(fLight - fPosition);
    vec3 normal = normalize(fNormal);
    float shading = clamp(dot(normal,light),0,1) * 0.7 + 0.3;
    Color = texture( myTextureSampler, fUV);
    if(Color.g > max(Color.r, Color.b)) // simple greeenscreen effect
    {
        Color = fColor;  
    }
    Color = Color * shading;
    Color.a = fColor.a;
}