#include "Drawables/Floor.hpp"

//first and las tolumn are swaped, now culling shows only inside of cube;
const GLfloat erppm::Floor::vertexBufferData[] = { 
    -0.5f,-0.5f, 0.0f,
     0.5f,-0.5f, 0.0f,
     0.5f, 0.5f, 0.0f,
    -0.5f,-0.5f, 0.0f,
     0.5f, 0.5f, 0.0f,
    -0.5f, 0.5f, 0.0f,
};

erppm::Floor::Floor()
: oglu::Drawable()
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
        layout(location = 1) in vec3 vertexColor;

        layout(location = 0)uniform mat4 MVP;
        layout(location = 1)uniform vec4 position;
        layout(location = 2)uniform vec4 scale;
        layout(location = 3)uniform vec4 rotation;

        out vec3 fragmentColor;
        out vec4 vpos;

        void main(){	
            gl_Position =  MVP * ((scale * vec4(vertexPosition,1)) + position);
            fragmentColor = vertexColor;
            vpos = scale * vec4(vertexPosition,1) + position;
        }
    )END", R"END(

        #version 330 core

        in vec3 fragmentColor;
        in vec4 vpos;

        out vec4 color;

        void main(){

            if(vpos[0] * vpos[1] < 0){
                color = (mod(int(vpos[0]) + int(vpos[1]),2) == 0.0 ? vec4(0.2,0.2,0.2,1) : vec4(0.8,0.8,0.8,1));
            }
            else{
                color = (mod(int(vpos[0]) + int(vpos[1]),2) == 0.0 ? vec4(0.8,0.8,0.8,1) : vec4(0.2,0.2,0.2,1));
            }
        }

    )END");
}

void erppm::Floor::setBuffers() {
    bindBuffers();

    GLuint vertexbuffer;    
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData), vertexBufferData, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(
        0,                  // attribute. No particular reason for 0, but must match the layout in the shader.
        3,                  // size
        GL_FLOAT,           // type
        GL_FALSE,           // normalized?
        0,                  // stride
        (void*)0            // array buffer offset
    );
}

void erppm::Floor::draw(const glm::mat4& MVP) {
    bindProgram();
    bindBuffers();
    glUniformMatrix4fv(0, 1, GL_FALSE, &MVP[0][0]);
    glUniform4f(1, position[0],position[1],position[2],1);
    glUniform4f(2, scale[0],scale[1],scale[2],1);
    glUniform4f(3, rotation[0],rotation[1],rotation[2],1);
    glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
   
}
