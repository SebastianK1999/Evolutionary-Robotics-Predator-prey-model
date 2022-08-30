#include "Robotics/LaserSensor.hpp"

#include <iostream> 
#include <numeric>
#include <algorithm>
#include "glm/gtc/constants.hpp"

#include "Robotics/ObjectTypes.hpp"

erppm::LaserSensor::LaserSensor(const glm::vec4& _positionAtRobot, const glm::vec4& _rotationAtRobot)
: SensorBase("../res/obj_models/laser_sensor", _positionAtRobot, _rotationAtRobot)
, laserBeam()
{}

erppm::LaserSensor::LaserSensor(LaserSensor&& other)
: SensorBase(std::move(other))
, laserBeam(std::move(other.laserBeam))
{}

erppm::LaserSensor::~LaserSensor(){}

void erppm::LaserSensor::measure(const std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor, std::vector<double>::iterator& measurementDataIterator){
    double& itDistance    = *std::next(measurementDataIterator,0);
    double& itObjectType  = *std::next(measurementDataIterator,1);

    const glm::vec2 direction = {glm::cos(body.rotation[2]), glm::sin(body.rotation[2])};
    EObjectType robotType = EObjectTypeNone;
    float robotsDistance = INFINITY;
    for(std::vector<RobotBase*>::const_iterator iter = robots.begin(); iter != robots.end(); iter = std::next(iter)){
        if((*iter) != (RobotBase*)(this->parent)){
            float laser_a = direction.y/direction.x;
            float laser_b = this->body.position.y - laser_a * this->body.position.x;
            float targetRobot_a = -1/laser_a;
            float targetRobot_b = (*iter)->getPosition().y - targetRobot_a * (*iter)->getPosition().x;
            if(laser_a != targetRobot_a){
                glm::vec2 meetingPoint;
                meetingPoint.x = (targetRobot_b - laser_b)/(laser_a - targetRobot_a);
                meetingPoint.y = laser_a * meetingPoint.x + laser_b;
                float distanceFromTargetRobot = glm::length(meetingPoint - *(glm::vec2*)&((*iter)->getPosition()));
                if(distanceFromTargetRobot <= 1){
                    if (direction.x >= 0 & meetingPoint.x >= this->body.position.x || direction.x < 0 & meetingPoint.x < this->body.position.x){
                        float tempDistance = glm::length(meetingPoint - *(glm::vec2*)&(this->body.position)) - glm::sqrt((distanceFromTargetRobot-1) * (-distanceFromTargetRobot - 1)); // sqrt(-(X-x0)*(X-x1))
                        robotsDistance = (tempDistance < robotsDistance ? tempDistance : robotsDistance);
                    }
                }
            }
        }
    }
    itDistance   = robotsDistance;
    itObjectType = 28;

    float wallsDistance = INFINITY;
    for(std::vector<Wall>::const_iterator iter = walls.begin(); iter != walls.end(); iter = std::next(iter)){
        if(direction.x > 0){
            if(direction.y > 0){
                const glm::vec2 directionToCorner = {((*iter).position.x-(*iter).scale.x) - body.position.x, ((*iter).position.y-(*iter).scale.y) - body.position.y};
                if(direction.x / direction.y > directionToCorner.x / directionToCorner.y && directionToCorner.y >= 0){   
                    const float tempDistance = directionToCorner.y / direction.y;
                    const float xDestination = direction.x*tempDistance + body.position.x;
                    if(xDestination > ((*iter).position.x-(*iter).scale.x) && xDestination < ((*iter).position.x+(*iter).scale.x) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
                else{
                    const float tempDistance = directionToCorner.x / direction.x;
                    const float yDestination = direction.y*tempDistance + body.position.y;
                    if(yDestination > ((*iter).position.y-(*iter).scale.y) && yDestination < ((*iter).position.y+(*iter).scale.y) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
            }
            else{
                const glm::vec2 directionToCorner = {((*iter).position.x-(*iter).scale.x) - body.position.x, ((*iter).position.y+(*iter).scale.y) - body.position.y};
                if(direction.x / direction.y < directionToCorner.x / directionToCorner.y && directionToCorner.y <= 0){     
                    const float tempDistance = directionToCorner.y / direction.y;
                    const float xDestination = direction.x*tempDistance + body.position.x;
                    if(xDestination > ((*iter).position.x-(*iter).scale.x) && xDestination < ((*iter).position.x+(*iter).scale.x) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
                else{
                    const float tempDistance = directionToCorner.x / direction.x;
                    const float yDestination = direction.y*tempDistance + body.position.y;
                    if(yDestination > ((*iter).position.y-(*iter).scale.y) && yDestination < ((*iter).position.y+(*iter).scale.y) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
            }
        }
        else{
            if(direction.y > 0){
                const glm::vec2 directionToCorner = {((*iter).position.x+(*iter).scale.x) - body.position.x,((*iter).position.y-(*iter).scale.y) - body.position.y};
                if(direction.x / direction.y < directionToCorner.x / directionToCorner.y && directionToCorner.y >= 0){   
                    const float tempDistance = directionToCorner.y / direction.y;
                    const float xDestination = direction.x*tempDistance + body.position.x;
                    if(xDestination > ((*iter).position.x-(*iter).scale.x) && xDestination < ((*iter).position.x+(*iter).scale.x) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
                else{
                    const float tempDistance = directionToCorner.x / direction.x;
                    const float yDestination = direction.y*tempDistance + body.position.y;
                    if(yDestination > ((*iter).position.y-(*iter).scale.y) && yDestination < ((*iter).position.y+(*iter).scale.y) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
            }
            else{
                const glm::vec2 directionToCorner = {((*iter).position.x+(*iter).scale.x) - body.position.x, ((*iter).position.y+(*iter).scale.y) - body.position.y};
                if(direction.x / direction.y > directionToCorner.x / directionToCorner.y && directionToCorner.y <= 0){   
                    const float tempDistance = directionToCorner.y / direction.y;
                    const float xDestination = direction.x*tempDistance + body.position.x;
                    if(xDestination > ((*iter).position.x-(*iter).scale.x) && xDestination < ((*iter).position.x+(*iter).scale.x) && tempDistance > -0.5){
                        wallsDistance = std::min(wallsDistance,tempDistance);
                    }
                }
                else{
                    const float tempDistance = directionToCorner.x / direction.x;
                    const float yDestination = direction.y*tempDistance + body.position.y;
                    if(yDestination > ((*iter).position.y-(*iter).scale.y) && yDestination < ((*iter).position.y+(*iter).scale.y) && tempDistance > -0.5){
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
            const glm::vec2 directionToCorner = {floor.scale.x - body.position.x, floor.scale.y - body.position.y};
            if(direction.x / direction.y > directionToCorner.x / directionToCorner.y){
                floorDistance = directionToCorner.x / direction.x;
            }
            else{
                floorDistance = directionToCorner.y / direction.y;
            }
        }
        else{
            const glm::vec2 directionToCorner = {floor.scale.x - body.position.x, -floor.scale.y - body.position.y};
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
            const glm::vec2 directionToCorner = {-floor.scale.x - body.position.x, floor.scale.y - body.position.y};
            if(direction.x / direction.y > directionToCorner.x / directionToCorner.y){
                floorDistance = directionToCorner.y / direction.y;
            }
            else{
                floorDistance = directionToCorner.x / direction.x;
            }
        }
        else{
            const glm::vec2 directionToCorner = {-floor.scale.x - body.position.x, -floor.scale.y - body.position.y};
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
    laserBeam.scale.x = itDistance;
}

void erppm::LaserSensor::draw(const glm::mat4& MVP, const glm::vec3& light) {
    body.draw(MVP, light);
    laserBeam.position = getPosition();
    laserBeam.rotation = getRotation();
    laserBeam.draw(MVP, getPrimaryColor());
}

const size_t erppm::LaserSensor::getMeasurementDataSize() const noexcept{
    return 2;
}