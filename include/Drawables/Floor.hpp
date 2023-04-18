
# pragma once 

#include "oglUtil/drawableSystem/DrawableInstance.hpp"

namespace erppm
{
    class Floor : public oglu::DrawableInstance
    {
    public:
        static constexpr char key[] = "erppm::Floor";
        static void drawInstances(const glm::mat4& MVP, const glm::vec3& light);
        oglu::RegisteredDrawable& getRegistry() const noexcept override;
        Floor();
        Floor(const Floor& other)             = default;
        Floor(Floor&& other)                  = default;
        Floor& operator=(const Floor& other)  = default;
        Floor& operator=(Floor&& other)       = default;
        virtual ~Floor()                      = default;

    private:
        static std::shared_ptr<oglu::RegisteredDrawable> staticRegistryPointer;

    };
}

