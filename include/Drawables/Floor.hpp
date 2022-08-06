
# pragma once 

#include "oglUtil/drawables/Drawable.hpp"

namespace erppm
{
    class Floor : public virtual oglu::Drawable {
    public:
        Floor();
        void setShaders();
        void setBuffers();
        void draw(const glm::mat4& MVP);
    private:
        static const GLfloat vertexBufferData[];
    };
    
}

