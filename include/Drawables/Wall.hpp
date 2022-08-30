#pragma once

#include "oglUtil/drawables/Drawable.hpp"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

namespace erppm{
    class Wall : public oglu::Drawable {
    public:
        glm::vec3 primaryColor;
        Wall(Wall&& other);
        Wall(const Wall& other);
        Wall();
        void setShaders();
        void setBuffers();
        void draw(const glm::mat4& MVP, const glm::vec3& light) const;
        void setColor(float r, float g, float b);
    private:
        static const GLfloat vertexBufferData[];
        static const GLfloat normalBufferData[];
        
    };
}