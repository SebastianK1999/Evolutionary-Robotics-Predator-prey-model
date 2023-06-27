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
    vec4 cDark = fColor * shading*0.3;
    vec4 cLight = fColor * (shading * 0.3 + 0.7);
    Color = texture( myTextureSampler, fUV);
    if(fPosition[0] * fPosition[1] < 0){
        Color = (mod(int(fPosition[0])/2 + int(fPosition[1])/2,2) == 0.0 ? cDark : cLight);
    }
    else{
        Color = (mod(int(fPosition[0])/2 + int(fPosition[1])/2,2) == 0.0 ? cLight : cDark);
    }
    Color = Color;
    Color.a = fColor.a;
}