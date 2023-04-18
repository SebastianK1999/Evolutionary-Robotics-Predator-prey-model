#pragma once

#include "oglUtil/drawableSystem/DrawableInstance.hpp"

namespace erppm
{
    class LaserBeam : public oglu::DrawableInstance 
    {
    public:
        static constexpr char key[] = "erppm::LaserBeam";
        static void drawInstances(const glm::mat4& MVP, const glm::vec3& light);
        oglu::RegisteredDrawable& getRegistry() const noexcept override;
        LaserBeam();
        LaserBeam(const LaserBeam& other)             = default;
        LaserBeam(LaserBeam&& other)                  = default;
        LaserBeam& operator=(const LaserBeam& other)  = default;
        LaserBeam& operator=(LaserBeam&& other)       = default;
        virtual ~LaserBeam()                          = default;

        static std::shared_ptr<oglu::RegisteredDrawable> staticRegistryPointer;
    private:

    };
}
