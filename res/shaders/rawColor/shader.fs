in vec4 fColor;
in vec2 fUV;

out vec4 Color;

uniform sampler2D myTextureSampler;

void main()
{
    Color = fColor;
    Color.a = fColor.a;
}