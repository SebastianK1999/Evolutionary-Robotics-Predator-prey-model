out vec4 fColor;
out vec2 fUV;

void main()
{	
    vec3 vertex = vVertex;
    vertex = vertex * vScale;
    vertex = rotate(vertex, vRotation);
    vertex = vertex + vPosition;

    gl_Position =  vMVP * vec4(vertex,1);
    fColor = vColor;
    fUV = vUV;
}

