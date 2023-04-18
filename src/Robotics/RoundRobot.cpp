#include <cassert>
#include <filesystem>

#include "glm/gtc/constants.hpp"

#include "Robotics/RoundRobot.hpp"

const std::filesystem::path erppm::RoundRobot::modelPath = std::filesystem::path() / ".." / "res" / "obj_models" / "round_robot";

void erppm::RoundRobot::draw(const glm::mat4& MVP, const glm::vec3& light)
{
    oglu::Mesh::drawInstances(erppm::RoundRobot::modelPath, MVP, light);
}

erppm::RoundRobot::RoundRobot()
: RobotBase(modelPath)
{
    auto& registry = oglu::Mesh::getRegistry(erppm::RoundRobot::modelPath);
    if(registry.size() == 1)
    {
        registry.loadShaders(std::filesystem::path() / ".." / "res" / "shaders" / "greenScreenTexture");
    }
}

erppm::RoundRobot::~RoundRobot(){}

void erppm::RoundRobot::run(double time, const std::vector<double>& controlInput) {
    assert(controlInput.size() == this->getControlInputSize());
    constexpr double maxRps                 = 5.0;
    constexpr double wheelRadius            = 0.1;
    constexpr double wheelPlacementRadius   = 0.9; 
    constexpr double friction               = 15; // friction < 1/time

    const float left_v  = controlInput[0] * glm::two_pi<double>() * wheelRadius * maxRps;
    const float right_v = controlInput[1] * glm::two_pi<double>() * wheelRadius * maxRps;

    const float fromMotorAngularVelocity = (right_v - left_v) / wheelPlacementRadius;
    angularVelocity += (fromMotorAngularVelocity - angularVelocity) * friction * time;
    body.getRotation().z += angularVelocity * time;

    const float fromMotorScalarVelocity = (right_v+left_v);
    const glm::dvec2 fromMotorVelocity = {fromMotorScalarVelocity * cosf(body.getRotation().z), fromMotorScalarVelocity * sinf(body.getRotation().z)};
    velocity += (fromMotorVelocity - velocity) * friction * time;
    body.getPosition().x += velocity.x * time;
    body.getPosition().y += velocity.y * time;
    

    //* If momentum wasn't a vector 
    // float fromMotorVelocity = (right_v+left_v);
    // fromMotorVelocity = (fromMotorVelocity - momentum / mass) * friction * time;
    // momentum += (fromMotorVelocity * mass);
    // body.getPosition().x += cosf(body.getRotation().z) * momentum / mass * time;
    // body.getPosition().y += sinf(body.getRotation().z) * momentum / mass * time;
}

const size_t erppm::RoundRobot::getControlInputSize() const noexcept{
    return 2;
}