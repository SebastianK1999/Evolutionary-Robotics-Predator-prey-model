#include "Robotics/SensorBase.hpp"

erppm::SensorBase::SensorBase(const std::string& modelPath, const glm::vec4& _positionAtRobot, const glm::vec4& _rotationAtRobot)
: positionAtRobot(_positionAtRobot)
, rotationAtRobot(_rotationAtRobot)
, body(modelPath)
, parent(nullptr)
{}

erppm::SensorBase::SensorBase(SensorBase&& other)
: positionAtRobot(std::move(other.positionAtRobot))
, rotationAtRobot(std::move(other.rotationAtRobot))
, body(std::move(other.body))
, parent(std::move(other.parent))
{}

erppm::SensorBase::~SensorBase(){}

void erppm::SensorBase::draw(const glm::mat4& MVP, const glm::vec3& light) {
    body.draw(MVP, light);
}

void erppm::SensorBase::updateLocation(glm::vec4 robotPosition, glm::vec4 robotRotation){
    body.rotation = robotRotation + rotationAtRobot;
    body.position = robotPosition + glm::vec4(
        positionAtRobot.x * glm::cos(robotRotation[2]) - positionAtRobot.y * glm::sin(robotRotation[2]),
        positionAtRobot.x * glm::sin(robotRotation[2]) + positionAtRobot.y * glm::cos(robotRotation[2]),
        positionAtRobot.z,
        0
    );
}

// void erppm::SensorBase::measure(const std::vector<double>::iterator controlInputIterator, std::vector<double>::iterator measurementDataIterator){

// }

glm::vec4& erppm::SensorBase::getPosition(){
    return body.position;
}

glm::vec4& erppm::SensorBase::getRotation(){
    return body.rotation;
}

glm::vec4& erppm::SensorBase::getScale(){
    return body.scale;
}

glm::vec3& erppm::SensorBase::getPrimaryColor(){
    return body.primaryColor;
}

const size_t erppm::SensorBase::getMeasurementDataSize() const noexcept{
    return 0;
}