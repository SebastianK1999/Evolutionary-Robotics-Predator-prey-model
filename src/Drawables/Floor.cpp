#include <vector>

#include "oglUtil/drawableSystem/DrawableRegistry.hpp"
#include "oglUtil/drawableSystem/ShaderCollection.hpp"

#include "Drawables/Floor.hpp"

static const std::vector<glm::vec3> gs_VertexBuffer = { 
    glm::vec3( 1.0f, 1.0f, 0.0f),
    glm::vec3(-1.0f, 1.0f, 0.0f),
    glm::vec3( 1.0f,-1.0f, 0.0f),

    glm::vec3(-1.0f,-1.0f, 0.0f),
    glm::vec3( 1.0f,-1.0f, 0.0f),
    glm::vec3(-1.0f, 1.0f, 0.0f),
};

static const std::vector<glm::vec3> gs_NormalBuffer = { 
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),

    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
    glm::vec3(0.0f, 0.0f, 1.0f),
};

std::shared_ptr<oglu::RegisteredDrawable> erppm::Floor::staticRegistryPointer = nullptr;

void erppm::Floor::drawInstances(const glm::mat4& MVP, const glm::vec3& light)
{
    staticRegistryPointer->drawInstances(MVP, light);
}

oglu::RegisteredDrawable& erppm::Floor::getRegistry() const noexcept 
{
    return *(registryPointer);
}

erppm::Floor::Floor()
{
    if(staticRegistryPointer == nullptr)
    {
        staticRegistryPointer = oglu::DrawableRegistry::registerDrawable
        (
            erppm::Floor::key,
            gs_VertexBuffer,
            {},
            gs_NormalBuffer,
            {},
            {},
            0,
            0,
            oglu::ShaderCollection::basicVertexShader,
            oglu::ShaderCollection::basicFragmentShader
        );
        staticRegistryPointer->loadShaders(std::filesystem::path() / ".." / "res" / "shaders" / "checkers");
    }
    registryPointer = staticRegistryPointer;
    instancePointer = registryPointer->addInstance();
}

