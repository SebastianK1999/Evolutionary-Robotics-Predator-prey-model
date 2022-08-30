#include "Robotics/RoundRobot.hpp"

#include "glm/gtc/constants.hpp"

erppm::RoundRobot::RoundRobot()
: RobotBase("../res/obj_models/round_robot")
{}

erppm::RoundRobot::~RoundRobot(){}

void erppm::RoundRobot::run(double time, const std::vector<double>& controlInput) {
    constexpr double maxRps                 = 5.0;
    constexpr double wheelRadius            = 0.1;
    constexpr double wheelPlacementRadius   = 0.9; 
    constexpr double friction               = 15; // friction < 1/time

    const float left_v  = controlInput[0] * glm::two_pi<double>() * wheelRadius * maxRps;
    const float right_v = controlInput[1] * glm::two_pi<double>() * wheelRadius * maxRps;

    const float fromMotorAngularVelocity = (right_v - left_v) / wheelPlacementRadius;
    angularVelocity += (fromMotorAngularVelocity - angularVelocity) * friction * time;
    body.rotation[2] += angularVelocity * time;

    const float fromMotorScalarVelocity = (right_v+left_v);
    const glm::dvec2 fromMotorVelocity = {fromMotorScalarVelocity * cosf(body.rotation[2]), fromMotorScalarVelocity * sinf(body.rotation[2])};
    velocity += (fromMotorVelocity - velocity) * friction * time;
    body.position.x += velocity.x * time;
    body.position.y += velocity.y * time;
    

    //* If momentum wasn't a vector 
    // float fromMotorVelocity = (right_v+left_v);
    // fromMotorVelocity = (fromMotorVelocity - momentum / mass) * friction * time;
    // momentum += (fromMotorVelocity * mass);
    // body.position.x += cosf(body.rotation[2]) * momentum / mass * time;
    // body.position.y += sinf(body.rotation[2]) * momentum / mass * time;
}

const size_t erppm::RoundRobot::getControlInputSize() const noexcept{
    return 2;
}