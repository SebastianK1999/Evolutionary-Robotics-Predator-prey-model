#include "Robotics/LaserSensor.hpp"

#include <iostream> 
#include <numeric>
#include <algorithm>
#include "glm/gtc/constants.hpp"

#include "Robotics/ObjectTypes.hpp"

const std::filesystem::path erppm::LaserSensor::modelPath = std::filesystem::path() / ".." / "res" / "obj_models" / "laser_sensor";

erppm::LaserSensor::LaserSensor
(
    const glm::vec3& _positionAtRobot,
    const glm::vec3& _rotationAtRobot,
    const std::array<double,erppm::LaserSensor::SIZE_OF_EVOLUTIONARY_DATA>& _evolutionaryData
)
: SensorBase(modelPath, _positionAtRobot, _rotationAtRobot)
, laserBeam()
{
    evolutionaryData = std::vector<double>(_evolutionaryData.data(), _evolutionaryData.data() + SIZE_OF_EVOLUTIONARY_DATA);
    auto& registry = oglu::Mesh::getRegistry(erppm::LaserSensor::modelPath);
    if(registry.size() == 1)
    {
        registry.loadShaders(std::filesystem::path() / ".." / "res" / "shaders" / "greenScreenTexture");
    }
}

erppm::LaserSensor::LaserSensor(LaserSensor&& other)
: SensorBase(std::move(other))
, laserBeam(std::move(other.laserBeam))
{
    evolutionaryData.resize(2);
}

erppm::LaserSensor::~LaserSensor(){}

void erppm::LaserSensor::measure(const std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor, std::vector<double>::iterator& measurementDataIterator){
    double& itDistance    = *std::next(measurementDataIterator,0);
    double& itObjectType  = *std::next(measurementDataIterator,1);

    const glm::vec2 direction = {glm::cos(body.getRotation().z), glm::sin(body.getRotation().z)};
    EObjectType robotType = EObjectTypeNone;
    float robotsDistance = INFINITY;
    for(std::vector<RobotBase*>::const_iterator iter = robots.begin(); iter != robots.end(); iter = std::next(iter)){
        if((*iter) != (RobotBase*)(this->parent)){
            float laser_a = direction.y/direction.x;
            float laser_b = this->body.getPosition().y - laser_a * this->body.getPosition().x;
            float targetRobot_a = -1/laser_a;
            float targetRobot_b = (*iter)->getPosition().y - targetRobot_a * (*iter)->getPosition().x;
            if(laser_a != targetRobot_a){
                glm::vec2 meetingPoint;
                meetingPoint.x = (targetRobot_b - laser_b)/(laser_a - targetRobot_a);
                meetingPoint.y = laser_a * meetingPoint.x + laser_b;
                float distanceFromTargetRobot = glm::length(meetingPoint - *(glm::vec2*)&((*iter)->getPosition()));
                if(distanceFromTargetRobot <= 1){
                    if (((direction.x >= 0) && (meetingPoint.x >= this->body.getPosition().x)) || ((direction.x < 0) && (meetingPoint.x < this->body.getPosition().x))){
                        float tempDistance = glm::length(meetingPoint - static_cast<glm::vec2>(this->body.getPosition())) - glm::sqrt((distanceFromTargetRobot-1) * (-distanceFromTargetRobot - 1)); // sqrt(-(X-x0)*(X-x1))
                        if(tempDistance < robotsDistance){
                            robotsDistance = tempDistance;
                            robotType = (*iter)->type;
                        }
                    }
                }
            }
        }
    }
    itObjectType = robotType;
    itDistance   = robotsDistance;

    float wallsDistance = INFINITY;
    for(std::vector<Wall>::const_iterator iter = walls.begin(); iter != walls.end(); iter = std::next(iter)){
        if(direction.x > 0){
            if(direction.y > 0){
                const glm::vec2 directionToCorner = {((*iter).getPosition().x-(*iter).getScale().x) - body.getPosition().x, ((*iter).getPosition().y-(*iter).getScale().y) - body.getPosition().y};
                if(direction.x / direction.y > directionToCorner.x / directionToCorner.y && directionToCorner.y >= 0){   
                    const float tempDistance = directionToCorner.y / direction.y;
                    const float xDestination = direction.x*tempDistance + body.getPosition().x;
                    if(xDestination > ((*iter).getPosition().x-(*iter).getScale().x) && xDestination < ((*iter).getPosition().x+(*iter).getScale().x) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
                else{
                    const float tempDistance = directionToCorner.x / direction.x;
                    const float yDestination = direction.y*tempDistance + body.getPosition().y;
                    if(yDestination > ((*iter).getPosition().y-(*iter).getScale().y) && yDestination < ((*iter).getPosition().y+(*iter).getScale().y) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
            }
            else{
                const glm::vec2 directionToCorner = {((*iter).getPosition().x-(*iter).getScale().x) - body.getPosition().x, ((*iter).getPosition().y+(*iter).getScale().y) - body.getPosition().y};
                if(direction.x / direction.y < directionToCorner.x / directionToCorner.y && directionToCorner.y <= 0){     
                    const float tempDistance = directionToCorner.y / direction.y;
                    const float xDestination = direction.x*tempDistance + body.getPosition().x;
                    if(xDestination > ((*iter).getPosition().x-(*iter).getScale().x) && xDestination < ((*iter).getPosition().x+(*iter).getScale().x) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
                else{
                    const float tempDistance = directionToCorner.x / direction.x;
                    const float yDestination = direction.y*tempDistance + body.getPosition().y;
                    if(yDestination > ((*iter).getPosition().y-(*iter).getScale().y) && yDestination < ((*iter).getPosition().y+(*iter).getScale().y) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
            }
        }
        else{
            if(direction.y > 0){
                const glm::vec2 directionToCorner = {((*iter).getPosition().x+(*iter).getScale().x) - body.getPosition().x,((*iter).getPosition().y-(*iter).getScale().y) - body.getPosition().y};
                if(direction.x / direction.y < directionToCorner.x / directionToCorner.y && directionToCorner.y >= 0){   
                    const float tempDistance = directionToCorner.y / direction.y;
                    const float xDestination = direction.x*tempDistance + body.getPosition().x;
                    if(xDestination > ((*iter).getPosition().x-(*iter).getScale().x) && xDestination < ((*iter).getPosition().x+(*iter).getScale().x) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
                else{
                    const float tempDistance = directionToCorner.x / direction.x;
                    const float yDestination = direction.y*tempDistance + body.getPosition().y;
                    if(yDestination > ((*iter).getPosition().y-(*iter).getScale().y) && yDestination < ((*iter).getPosition().y+(*iter).getScale().y) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
            }
            else{
                const glm::vec2 directionToCorner = {((*iter).getPosition().x+(*iter).getScale().x) - body.getPosition().x, ((*iter).getPosition().y+(*iter).getScale().y) - body.getPosition().y};
                if(direction.x / direction.y > directionToCorner.x / directionToCorner.y && directionToCorner.y <= 0){   
                    const float tempDistance = directionToCorner.y / direction.y;
                    const float xDestination = direction.x*tempDistance + body.getPosition().x;
                    if(xDestination > ((*iter).getPosition().x-(*iter).getScale().x) && xDestination < ((*iter).getPosition().x+(*iter).getScale().x) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
                else{
                    const float tempDistance = directionToCorner.x / direction.x;
                    const float yDestination = direction.y*tempDistance + body.getPosition().y;
                    if(yDestination > ((*iter).getPosition().y-(*iter).getScale().y) && yDestination < ((*iter).getPosition().y+(*iter).getScale().y) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
            }
        }
    }
    if(wallsDistance < itDistance){
        itDistance   = wallsDistance;
        itObjectType = EObjectTypeWall;
    }

    float floorDistance = INFINITY;
    if(direction.x > 0){
        if(direction.y > 0){
            const glm::vec2 directionToCorner = {floor.getScale().x - body.getPosition().x, floor.getScale().y - body.getPosition().y};
            if(direction.x / direction.y > directionToCorner.x / directionToCorner.y){
                floorDistance = directionToCorner.x / direction.x;
            }
            else{
                floorDistance = directionToCorner.y / direction.y;
            }
        }
        else{
            const glm::vec2 directionToCorner = {floor.getScale().x - body.getPosition().x, -floor.getScale().y - body.getPosition().y};
            if(direction.x / direction.y > directionToCorner.x / directionToCorner.y){
                floorDistance = directionToCorner.y / direction.y;
            }
            else{
                floorDistance = directionToCorner.x / direction.x;
            }
        }
    }
    else{
        if(direction.y > 0){
            const glm::vec2 directionToCorner = {-floor.getScale().x - body.getPosition().x, floor.getScale().y - body.getPosition().y};
            if(direction.x / direction.y > directionToCorner.x / directionToCorner.y){
                floorDistance = directionToCorner.y / direction.y;
            }
            else{
                floorDistance = directionToCorner.x / direction.x;
            }
        }
        else{
            const glm::vec2 directionToCorner = {-floor.getScale().x - body.getPosition().x, -floor.getScale().y - body.getPosition().y};
            if(direction.x / direction.y > directionToCorner.x / directionToCorner.y){
                floorDistance = directionToCorner.x / direction.x;
            }
            else{
                floorDistance = directionToCorner.y / direction.y;
            }
        }
    }

    if(floorDistance < itDistance){
        itDistance   = floorDistance;
        itObjectType = EObjectTypeWall; // or EObjectTypeEdge for easier recognition
    }
    laserBeam.getScale().x = itDistance;
    itDistance /= 30.0f; // TODO normalization
}

void erppm::LaserSensor::draw(const glm::mat4& MVP, const glm::vec3& light)
{
    oglu::Mesh::drawInstances(erppm::LaserSensor::modelPath, MVP, light);
    erppm::LaserBeam::drawInstances(MVP, light);
}

const size_t erppm::LaserSensor::getMeasurementDataSize() const noexcept{
    return 2;
}

void erppm::LaserSensor::reinitializeDrawables(){
    // TODO
    // body.reinitialize();
    // laserBeam.reinitialize();
}

void erppm::LaserSensor::updateDecorator()
{
    laserBeam.getPosition() = body.getPosition();
    laserBeam.getRotation().z = body.getRotation().z;
}


void erppm::LaserSensor::updateDecoratorColors(const glm::vec4& color)
{
    laserBeam.getColor() = color;
}