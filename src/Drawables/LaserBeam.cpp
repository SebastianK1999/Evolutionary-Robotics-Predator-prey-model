#include "Drawables/LaserBeam.hpp"

#include <utility>

//first and las column are swapped, now culling shows only inside of cube;
const GLfloat erppm::LaserBeam::vertexBufferData[] = { 
     0.00f,  0.03f,  0.00f,
     0.00f, -0.03f,  0.00f,
     1.00f,  0.03f,  0.00f,
     0.00f, -0.03f,  0.00f,
     1.00f,  0.03f,  0.00f,
     1.00f, -0.03f,  0.00f,
     
     0.00f,  0.03f,  0.00f,
     1.00f,  0.03f,  0.00f,
     0.00f, -0.03f,  0.00f,
     0.00f, -0.03f,  0.00f,
     1.00f, -0.03f,  0.00f,
     1.00f,  0.03f,  0.00f,
     
     0.00f,  0.00f,  0.03f,
     0.00f,  0.00f, -0.03f,
     1.00f,  0.00f,  0.03f,
     0.00f,  0.00f, -0.03f,
     1.00f,  0.00f,  0.03f,
     1.00f,  0.00f, -0.03f,
     
     0.00f,  0.00f,  0.03f,
     1.00f,  0.00f,  0.03f,
     0.00f,  0.00f, -0.03f,
     0.00f,  0.00f, -0.03f,
     1.00f,  0.00f, -0.03f,
     1.00f,  0.00f,  0.03f,
};

erppm::LaserBeam::LaserBeam(const LaserBeam& other)
: oglu::Drawable(other)
, oglu::DrawableBase(other)
, primaryColor(other.primaryColor)
{
    setShaders();
    setBuffers();
}

erppm::LaserBeam::LaserBeam(LaserBeam&& other)
: oglu::Drawable(std::move(other))
, oglu::DrawableBase(std::move(other))
, primaryColor(std::move(other.primaryColor))
{
    setShaders();
    setBuffers();
}

erppm::LaserBeam::LaserBeam()
: oglu::Drawable()
, primaryColor()
{
    setShaders();
    setBuffers();
}

void erppm::LaserBeam::setShaders() {
    compileShaders(R"END(

        #version 330 core
        #extension GL_ARB_explicit_uniform_location : require
        #extension GL_ARB_shading_language_420pack : require

        layout(location = 0) in vec3 vertexPosition;

        layout(location = 0) uniform mat4 MVP;
        layout(location = 1) uniform vec4 position;
        layout(location = 2) uniform vec4 scale;
        layout(location = 3) uniform vec4 rotation;
        layout(location = 4) uniform vec3 vCol;


        out vec3 fragmentColor;
        out vec3 fPos;
        out vec3 fCol;

        void main(){	
            vec4 verPos = vec4(vertexPosition,0)*scale;
            float x = verPos.x;
            float y = verPos.y;
            verPos.x = x * cos(rotation.z) - y * sin(rotation.z);
            verPos.y = x * sin(rotation.z) + y * cos(rotation.z);
            //verPos.z = ;
            verPos = (verPos + position);
            gl_Position =  MVP * verPos;
            fPos = verPos.xyz;
            fCol = vCol;
        }
    )END", R"END(

        #version 330 core
        in vec3 fPos;
        in vec3 fCol;

        out vec4 color;

        void main(){
            color = vec4(fCol,1);
        }

    )END");
}

void erppm::LaserBeam::setBuffers() {
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

void erppm::LaserBeam::draw(const glm::mat4& MVP, const glm::vec3 color) const {
    bindProgram();
    bindBuffers();
    glUniformMatrix4fv(0, 1, GL_FALSE, &MVP[0][0]);
    glUniform4f(1, position[0],position[1],position[2],1);
    glUniform4f(2, scale[0],scale[1],scale[2],1);
    glUniform4f(3, rotation[0],rotation[1],rotation[2],1);
    glUniform3f(4, color.x, color.y, color.z);
    glDrawArrays(GL_TRIANGLES, 0, 12*3); // 12*3 indices starting at 0 -> 12 triangles
    
}

void erppm::LaserBeam::reinitialize(){
	glGenVertexArrays(1, &(vaoId));
	bindProgram();
	setShaders();
	setBuffers();
}
