#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <memory>
#include <string>

#include <GL/glew.h>
#include <glm/glm.hpp>

#include "oglUtil/drawableSystem/DrawableInstance.hpp"



namespace erppm
{
    class Wall : public oglu::DrawableInstance
    {
    public:
        static constexpr char key[] = "erppm::Wall";
        static void drawInstances(const glm::mat4& MVP, const glm::vec3& light);
        // static draw(const glm::mat4& MVP, const glm::vec3& light);
        oglu::RegisteredDrawable& getRegistry() const noexcept override;
        Wall();
        Wall(const Wall& other)             = default;
        Wall(Wall&& other)                  = default;
        Wall& operator=(const Wall& other)  = default;
        Wall& operator=(Wall&& other)       = default;
        virtual ~Wall()                     = default;

    private:
        static std::shared_ptr<oglu::RegisteredDrawable> staticRegistryPointer;

    };
}