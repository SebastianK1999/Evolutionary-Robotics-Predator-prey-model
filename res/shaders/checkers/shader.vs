out vec3 fPosition;
out vec3 fNormal;
out vec3 fLight;
out vec4 fColor;
out vec2 fUV;

void main()
{	
    vec3 vertex = vVertex;
    vertex = vertex * vScale;
    vertex = rotate(vertex, vRotation);
    vertex = vertex + vPosition;

    gl_Position =  vMVP * vec4(vertex,1);
    fPosition = vertex;
    fNormal = rotate(vNormal, vRotation);
    fLight = vLight;
    fColor = vColor;
    fUV = vUV;
}

