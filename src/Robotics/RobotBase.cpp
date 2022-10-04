#include "Robotics/RobotBase.hpp"

#include <iostream>
#include <numeric>

erppm::RobotBase::RobotBase(const std::string& modePlath)
: body(modePlath)
, velocity(0,0)
, angularVelocity(0)
, type(EObjectTypeNone)
, sensorData()
, sensors()
{}

erppm::RobotBase::~RobotBase(){}

void erppm::RobotBase::draw(const glm::mat4& MVP, const glm::vec3& light) const {
    for(std::vector<SensorBase*>::const_iterator iter = sensors.begin();  iter != sensors.end(); iter=std::next(iter)){
        (*iter)->draw(MVP,light);
    }
    body.draw(MVP, light);
}

// void erppm::RobotBase::run(double time, const std::vector<double>& controlInput);

void erppm::RobotBase::updateSensorsPosition(){
    for(std::vector<SensorBase*>::const_iterator iter = sensors.begin(); iter != sensors.end(); iter=std::next(iter)){
        (*iter)->updateLocation(body.position, body.rotation);
    }
}

void erppm::RobotBase::collide(std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor){
    constexpr bool bounceOn = true;
    constexpr float robotRadius = 1;
    for(std::vector<RobotBase*>::iterator iter = robots.begin(); iter != robots.end(); iter=std::next(iter)){
        if((*iter) != this){
            glm::vec4& robot1Pos = this->getPosition();
            glm::vec4& robot2Pos = (*iter)->getPosition();
            glm::vec4 relCenter = (robot1Pos + robot2Pos)*0.5f;
            if(std::pow(robot1Pos.x - robot2Pos.x,2) + std::pow(robot1Pos.y - robot2Pos.y,2) < std::pow(1+1,2)){
                glm::vec4 direction = glm::normalize(robot1Pos - relCenter);
                robot1Pos = relCenter + direction;
                robot2Pos = relCenter - direction;
                // possible change of velocity, due to collision 
            }
        }
    }
    // Walls
    for(std::vector<Wall>::const_iterator wallIter = walls.begin(); wallIter != walls.end(); wallIter = std::next(wallIter)){
    
        glm::vec4 relativeCenter = (*wallIter).position - this->getPosition();
        //* With commented out code, scale x y has to be as close as possible 
        // if(abs(relativeCenter.x)*(*wallIter).scale.y < abs(relativeCenter.y)*(*wallIter).scale.x){
        //* Now scale has to be equal
        if(abs(relativeCenter.x)*(*wallIter).scale.y < abs(relativeCenter.y)*(*wallIter).scale.x){
            if(relativeCenter.y > 0 && (*wallIter).position.y - this->getPosition().y < (*wallIter).scale.y+robotRadius){
                this->getPosition().y = -((*wallIter).scale.y+robotRadius) + (*wallIter).position.y;
                if constexpr(bounceOn){
                    this->velocity.y *= -0.9;
                    this->velocity.x *= 0.9;
                }
            }
            else if(relativeCenter.y < 0 && (*wallIter).position.y - this->getPosition().y > -((*wallIter).scale.y+robotRadius)){
                this->getPosition().y = (*wallIter).scale.y+robotRadius + (*wallIter).position.y;
                if constexpr(bounceOn){
                    this->velocity.y *= -0.9;
                    this->velocity.x *= 0.9;
                }
            }
        }
        else{
            if(relativeCenter.x > 0 && (*wallIter).position.x - this->getPosition().x < (*wallIter).scale.x+robotRadius){
                this->getPosition().x = -((*wallIter).scale.x+robotRadius) + (*wallIter).position.x;
                if constexpr(bounceOn){
                    this->velocity.y *= 0.9;
                    this->velocity.x *= -0.9;
                }
            }
            else if(relativeCenter.x < 0 && (*wallIter).position.x - this->getPosition().x > -((*wallIter).scale.x+robotRadius)){
                this->getPosition().x = (*wallIter).scale.x+robotRadius + (*wallIter).position.x;
                if constexpr(bounceOn){
                    this->velocity.y *= 0.9;
                    this->velocity.x *= -0.9;
                }
            }
        }
    }
    // Floor
    if(this->getPosition().x - 1 < -floor.scale.x){
        this->getPosition().x = -floor.scale.x + 1;
        if constexpr(bounceOn){
            this->velocity.y *=  0.9;
            this->velocity.x *= -0.9;
        }
    }
    else if(this->getPosition().x + 1 > floor.scale.x){
        this->getPosition().x = floor.scale.x - 1;
        if constexpr(bounceOn){
            this->velocity.y *=  0.9;
            this->velocity.x *= -0.9;
        }
    }

    if(this->getPosition().y - 1 < -floor.scale.y){
        this->getPosition().y = -floor.scale.y + 1;
        if constexpr(bounceOn){
            this->velocity.y *= -0.9;
            this->velocity.x *=  0.9;
        }
    }
    else if(this->getPosition().y + 1 > floor.scale.y){
        this->getPosition().y = floor.scale.y - 1;
        if constexpr(bounceOn){
            this->velocity.y *= -0.9;
            this->velocity.x *=  0.9;
        }
    }
}

void erppm::RobotBase::measure(const std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor){
    std::vector<double>::iterator sensorDataIter = sensorData.begin();
    for(std::vector<SensorBase*>::const_iterator iter = sensors.begin(); iter != sensors.end(); iter=std::next(iter)){
        (*iter)->measure(robots, walls, floor, sensorDataIter);
        sensorDataIter = std::next(sensorDataIter,(*iter)->getMeasurementDataSize());
    }
}

const std::vector<double>& erppm::RobotBase::getSensorData() const {
    return sensorData;
}

glm::vec4& erppm::RobotBase::getPosition(){
    return body.position;
}

glm::vec4& erppm::RobotBase::getRotation(){
    return body.rotation;
}

glm::vec4& erppm::RobotBase::getScale(){
    return body.scale;
}

glm::vec3& erppm::RobotBase::getPrimaryColor(){
    return body.primaryColor;
}

const size_t erppm::RobotBase::getControlInputSize() const noexcept{
    return 0;
}

const size_t erppm::RobotBase::getSensorDataSize() const noexcept{
    return std::accumulate(sensors.begin(), sensors.end(), 0, [](int i, const erppm::SensorBase* o){return i + o->getMeasurementDataSize();});
}

void erppm::RobotBase::reinitializeDrawables(){
    body.reinitialize();
    for(std::vector<erppm::SensorBase *>::iterator iter = sensors.begin(); iter != sensors.end(); iter = std::next(iter)){
        (*iter)->reinitializeDrawables();
    }
}

void erppm::RobotBase::clearVelocity(){
    velocity = {0.0, 0.0};
    angularVelocity = 0.0;
}