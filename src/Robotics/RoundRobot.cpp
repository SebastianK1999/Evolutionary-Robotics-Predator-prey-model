#include <cassert>

#include "glm/gtc/constants.hpp"

#include "Robotics/RoundRobot.hpp"


erppm::RoundRobot::RoundRobot()
: RobotBase("../res/obj_models/round_robot")
, body("../res/obj_models/round_robot")
{
        // getBody() = Mesh("../res/obj_models/round_robot");

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
    getBody().rotation[2] += angularVelocity * time;

    const float fromMotorScalarVelocity = (right_v+left_v);
    const glm::dvec2 fromMotorVelocity = {fromMotorScalarVelocity * cosf(getBody().rotation[2]), fromMotorScalarVelocity * sinf(getBody().rotation[2])};
    velocity += (fromMotorVelocity - velocity) * friction * time;
    getBody().position.x += velocity.x * time;
    getBody().position.y += velocity.y * time;
    

    //* If momentum wasn't a vector 
    // float fromMotorVelocity = (right_v+left_v);
    // fromMotorVelocity = (fromMotorVelocity - momentum / mass) * friction * time;
    // momentum += (fromMotorVelocity * mass);
    // getBody().position.x += cosf(getBody().rotation[2]) * momentum / mass * time;
    // getBody().position.y += sinf(getBody().rotation[2]) * momentum / mass * time;
}

const size_t erppm::RoundRobot::getControlInputSize() const noexcept{
    return 2;
}

Mesh& erppm::RoundRobot::getBody() noexcept {
    return erppm::RoundRobot::getBody();
}

const Mesh& erppm::RoundRobot::getBody() const noexcept {
    return erppm::RoundRobot::getBody();
}