
# pragma once 

#include "oglUtil/drawables/Drawable.hpp"

namespace erppm
{
    class Floor : public virtual oglu::Drawable {
    public:
        glm::vec3 primaryColor;
        Floor();
        void setShaders();
        void setBuffers();
        void draw(const glm::mat4& MVP, const glm::vec3& light) const;
        void reinitialize();
    private:
        static const GLfloat vertexBufferData[];
    };
    
}

