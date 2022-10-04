#include "Drawables/Floor.hpp"

//first and las column are swapped, now culling shows only inside of cube;
const GLfloat erppm::Floor::vertexBufferData[] = { 
    -1.0f,-1.0f, 0.0f,
     1.0f,-1.0f, 0.0f,
     1.0f, 1.0f, 0.0f,
    -1.0f,-1.0f, 0.0f,
     1.0f, 1.0f, 0.0f,
    -1.0f, 1.0f, 0.0f,
};

erppm::Floor::Floor()
: oglu::Drawable()
, primaryColor()
{
    //GLfloat cross_color[3] = { 0.0, 1.0, 0.0 };
    setShaders();
    setBuffers();
}

void erppm::Floor::setShaders() {
    compileShaders(R"END(

        #version 330 core
        #extension GL_ARB_explicit_uniform_location : require
        #extension GL_ARB_shading_language_420pack : require

        layout(location = 0) in vec3 vertexPosition;

        layout(location = 0) uniform mat4 MVP;
        layout(location = 1) uniform vec4 position;
        layout(location = 2) uniform vec4 scale;
        layout(location = 3) uniform vec3 light;
        layout(location = 4) uniform vec3 vCol;


        out vec3 fragmentColor;
        out vec3 fPos;
        out vec3 fCol;
        out vec3 fLight;

        void main(){	
            vec4 verPos = position + vec4(vertexPosition,0)*scale;

            gl_Position =  MVP * verPos;
            fPos = verPos.xyz;
            fCol = vCol;
            fLight = light;
        }
    )END", R"END(

        #version 330 core
        in vec3 fPos;
        in vec3 fCol;
        in vec3 fLight;

        out vec4 color;

        void main(){
            vec3 l = normalize(fLight - fPos.xyz);
            vec3 n = vec3(0,0,1);
            float shading = clamp(dot(n,l),0,1);
            vec3 cDark = fCol * shading*0.3;
            vec3 cLight = fCol * (shading * 0.3 + 0.7);
            if(fPos[0] * fPos[1] < 0){
                color = (mod(int(fPos[0]) + int(fPos[1]),2) == 0.0 ? vec4(cDark.r, cDark.g, cDark.b, 1.0) : vec4(cLight.r, cLight.g, cLight.b, 1.0));
            }
            else{
                color = (mod(int(fPos[0]) + int(fPos[1]),2) == 0.0 ? vec4(cLight.r, cLight.g, cLight.b, 1.0) : vec4(cDark.r, cDark.g, cDark.b, 1.0));
            }
        }

    )END");
}

void erppm::Floor::setBuffers() {
    bindBuffers();

    GLuint vertexBuffer;    
    glGenBuffers(1, &vertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
    glVertexAttribPointer(
        0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
}

void erppm::Floor::draw(const glm::mat4& MVP, const glm::vec3& light) const {
    bindProgram();
    bindBuffers();
    glUniformMatrix4fv(0, 1, GL_FALSE, &MVP[0][0]);
    glUniform4f(1, position[0],position[1],position[2],1);
    glUniform4f(2, scale[0],scale[1],scale[2],1);
    glUniform3f(3, light.x, light.y, light.z);
    glUniform3f(4, primaryColor.x, primaryColor.y, primaryColor.z);
    glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
    
}

void erppm::Floor::reinitialize(){
	glGenVertexArrays(1, &(vaoId));
	bindProgram();
	setShaders();
	setBuffers();
}

