#pragma once

#include "oglUtil/drawables/Drawable.hpp"

namespace erppm
{
    class LaserBeam : public virtual oglu::Drawable {
    public:
        glm::vec3 primaryColor;
        LaserBeam(const LaserBeam& other);
        LaserBeam(LaserBeam&& other);
        LaserBeam();
        void setShaders();
        void setBuffers();
        void draw(const glm::mat4& MVP, const glm::vec3 color) const;
        void reinitialize();
    private:
        static const GLfloat vertexBufferData[];
    };
    
}
