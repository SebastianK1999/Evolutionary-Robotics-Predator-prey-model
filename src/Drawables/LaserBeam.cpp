#include <vector>

#include "oglUtil/drawableSystem/DrawableRegistry.hpp"
#include "oglUtil/drawableSystem/ShaderCollection.hpp"

#include "Drawables/LaserBeam.hpp"

//first and las column are swapped, now culling shows only inside of cube;
static const std::vector<glm::vec3> gs_VertexBuffer = { 
    glm::vec3( 0.00f,  0.03f,  0.00f),
    glm::vec3( 0.00f, -0.03f,  0.00f),
    glm::vec3( 1.00f,  0.03f,  0.00f),
    glm::vec3( 0.00f, -0.03f,  0.00f),
    glm::vec3( 1.00f,  0.03f,  0.00f),
    glm::vec3( 1.00f, -0.03f,  0.00f),

    glm::vec3( 0.00f,  0.03f,  0.00f),
    glm::vec3( 1.00f,  0.03f,  0.00f),
    glm::vec3( 0.00f, -0.03f,  0.00f),
    glm::vec3( 0.00f, -0.03f,  0.00f),
    glm::vec3( 1.00f, -0.03f,  0.00f),
    glm::vec3( 1.00f,  0.03f,  0.00f),

    glm::vec3( 0.00f,  0.00f,  0.03f),
    glm::vec3( 0.00f,  0.00f, -0.03f),
    glm::vec3( 1.00f,  0.00f,  0.03f),
    glm::vec3( 0.00f,  0.00f, -0.03f),
    glm::vec3( 1.00f,  0.00f,  0.03f),
    glm::vec3( 1.00f,  0.00f, -0.03f),

    glm::vec3( 0.00f,  0.00f,  0.03f),
    glm::vec3( 1.00f,  0.00f,  0.03f),
    glm::vec3( 0.00f,  0.00f, -0.03f),
    glm::vec3( 0.00f,  0.00f, -0.03f),
    glm::vec3( 1.00f,  0.00f, -0.03f),
    glm::vec3( 1.00f,  0.00f,  0.03f),
};

std::shared_ptr<oglu::RegisteredDrawable> erppm::LaserBeam::staticRegistryPointer = nullptr;

void erppm::LaserBeam::drawInstances(const glm::mat4& MVP, const glm::vec3& light)
{
    if(staticRegistryPointer == nullptr)
    {
        return;
    }
    staticRegistryPointer->drawInstances(MVP, light);
}

oglu::RegisteredDrawable& erppm::LaserBeam::getRegistry() const noexcept 
{
    return *(registryPointer);
}

erppm::LaserBeam::LaserBeam()
{
    if(staticRegistryPointer == nullptr)
    {
        staticRegistryPointer = oglu::DrawableRegistry::registerDrawable
        (
            erppm::LaserBeam::key,
            gs_VertexBuffer,
            {},
            {},
            {},
            {},
            0,
            0,
            oglu::ShaderCollection::basicVertexShader,
            oglu::ShaderCollection::basicFragmentShader
        );
        erppm::LaserBeam::staticRegistryPointer->loadShaders(std::filesystem::path() / ".." / "res" / "shaders" / "rawColor");
    }
    registryPointer = staticRegistryPointer;
    instancePointer = registryPointer->addInstance();
}
