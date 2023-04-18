#include "Robotics/SensorBase.hpp"

erppm::SensorBase::SensorBase(const std::filesystem::path& modelPath, const glm::vec3& _positionAtRobot, const glm::vec3& _rotationAtRobot)
: positionAtRobot(_positionAtRobot)
, rotationAtRobot(_rotationAtRobot)
, evolutionaryData(0)
, body(modelPath)
, parent(nullptr)
{}

erppm::SensorBase::SensorBase(SensorBase&& other)
: positionAtRobot(std::move(other.positionAtRobot))
, rotationAtRobot(std::move(other.rotationAtRobot))
, evolutionaryData(std::move(other.evolutionaryData))
, body(std::move(other.body))
, parent(std::move(other.parent))
{}

erppm::SensorBase::~SensorBase(){}

void erppm::SensorBase::updateLocation(glm::vec3 robotPosition, glm::vec3 robotRotation){
    body.getRotation() = robotRotation + rotationAtRobot;
    body.getPosition() = robotPosition + glm::vec3(
        positionAtRobot.x * glm::cos(robotRotation.z) - positionAtRobot.y * glm::sin(robotRotation.z),
        positionAtRobot.x * glm::sin(robotRotation.z) + positionAtRobot.y * glm::cos(robotRotation.z),
        positionAtRobot.z
    );
    updateDecorator();
}

// void erppm::SensorBase::measure(const std::vector<double>::iterator controlInputIterator, std::vector<double>::iterator measurementDataIterator)
// {
// }

glm::vec3& erppm::SensorBase::getPosition(){
    return body.getPosition();
}

glm::vec3& erppm::SensorBase::getRotation(){
    return body.getRotation();
}

glm::vec3& erppm::SensorBase::getScale(){
    return body.getScale();
}

glm::vec4& erppm::SensorBase::getPrimaryColor(){
    return body.getColor();
}

const size_t erppm::SensorBase::getMeasurementDataSize() const noexcept{
    return 0;
}