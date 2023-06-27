#include <iostream>
#include <numeric>

#include "glm/gtc/constants.hpp"

#include "Robotics/LaserSensor.hpp"

#include "Robotics/RobotBase.hpp"

erppm::RobotBase::RobotBase(const std::filesystem::path& modePlath)
: body(modePlath)
, network(0,0,0,0,Jimmy::Methods::transFuncs::tanh, 0.1)
, velocity(0,0)
, angularVelocity(0)
, type(EObjectTypeNone)
, sensorData()
, sensors()
{}

erppm::RobotBase::~RobotBase(){}

// void erppm::RobotBase::run(double time, const std::vector<double>& controlInput);

void erppm::RobotBase::updateSensorsPosition(){
    for(std::vector<SensorBase*>::const_iterator iter = sensors.begin(); iter != sensors.end(); iter=std::next(iter)){
        (*iter)->updateLocation(body.getPosition(), body.getRotation());
    }
}

void erppm::RobotBase::collide(const std::vector<RobotBase*>& robots, const std::vector<Wall>& walls, const Floor& floor){
    constexpr bool bounceOn = true;
    constexpr float robotRadius = 1;
    for(std::vector<RobotBase*>::const_iterator iter = robots.begin(); iter != robots.end(); iter=std::next(iter)){
        if((*iter) != this){
            glm::vec3& robot1Pos = this->getPosition();
            glm::vec3& robot2Pos = (*iter)->getPosition();
            glm::vec3 relCenter = (robot1Pos + robot2Pos)*0.5f;
            if(std::pow(robot1Pos.x - robot2Pos.x,2) + std::pow(robot1Pos.y - robot2Pos.y,2) < std::pow(1+1,2)){
                glm::vec3 direction = glm::normalize(robot1Pos - relCenter);
                robot1Pos = relCenter + direction;
                robot2Pos = relCenter - direction;
                // possible change of velocity, due to collision 
            }
        }
    }
    // Walls
    for(std::vector<Wall>::const_iterator wallIter = walls.begin(); wallIter != walls.end(); wallIter = std::next(wallIter)){
        glm::vec3 relativeCenter = (*wallIter).getPosition() - this->getPosition();
        //* With commented out code, scale x y has to be as close as possible 
        // if(abs(relativeCenter.x)*(*wallIter).scale.y < abs(relativeCenter.y)*(*wallIter).scale.x){
        //* Now scale has to be equal
        if(abs(relativeCenter.x)*(*wallIter).getScale().y < abs(relativeCenter.y)*(*wallIter).getScale().x){
            if(relativeCenter.y > 0 && (*wallIter).getPosition().y - this->getPosition().y < (*wallIter).getScale().y+robotRadius){
                this->getPosition().y = -((*wallIter).getScale().y+robotRadius) + (*wallIter).getPosition().y;
                if constexpr(bounceOn){
                    this->velocity.y *= -0.9;
                    this->velocity.x *= 0.9;
                }
            }
            else if(relativeCenter.y < 0 && (*wallIter).getPosition().y - this->getPosition().y > -((*wallIter).getScale().y+robotRadius)){
                this->getPosition().y = (*wallIter).getScale().y+robotRadius + (*wallIter).getPosition().y;
                if constexpr(bounceOn){
                    this->velocity.y *= -0.9;
                    this->velocity.x *= 0.9;
                }
            }
        }
        else{
            if(relativeCenter.x > 0 && (*wallIter).getPosition().x - this->getPosition().x < (*wallIter).getScale().x+robotRadius){
                this->getPosition().x = -((*wallIter).getScale().x+robotRadius) + (*wallIter).getPosition().x;
                if constexpr(bounceOn){
                    this->velocity.y *= 0.9;
                    this->velocity.x *= -0.9;
                }
            }
            else if(relativeCenter.x < 0 && (*wallIter).getPosition().x - this->getPosition().x > -((*wallIter).getScale().x+robotRadius)){
                this->getPosition().x = (*wallIter).getScale().x+robotRadius + (*wallIter).getPosition().x;
                if constexpr(bounceOn){
                    this->velocity.y *= 0.9;
                    this->velocity.x *= -0.9;
                }
            }
        }
    }
    // Floor
    if(this->getPosition().x - 1 < -floor.getScale().x){
        this->getPosition().x = -floor.getScale().x + 1;
        if constexpr(bounceOn){
            this->velocity.y *=  0.9;
            this->velocity.x *= -0.9;
        }
    }
    else if(this->getPosition().x + 1 > floor.getScale().x){
        this->getPosition().x = floor.getScale().x - 1;
        if constexpr(bounceOn){
            this->velocity.y *=  0.9;
            this->velocity.x *= -0.9;
        }
    }

    if(this->getPosition().y - 1 < -floor.getScale().y){
        this->getPosition().y = -floor.getScale().y + 1;
        if constexpr(bounceOn){
            this->velocity.y *= -0.9;
            this->velocity.x *=  0.9;
        }
    }
    else if(this->getPosition().y + 1 > floor.getScale().y){
        this->getPosition().y = floor.getScale().y - 1;
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

glm::vec3& erppm::RobotBase::getPosition() const noexcept
{
    return body.getPosition();
}

glm::vec3& erppm::RobotBase::getRotation() const noexcept
{
    return body.getRotation();
}

glm::vec3& erppm::RobotBase::getScale() const noexcept
{
    return body.getScale();
}

glm::vec4& erppm::RobotBase::getPrimaryColor() const noexcept
{
    return body.getColor();
}

const size_t erppm::RobotBase::getControlInputSize() const noexcept{
    return 0;
}

const size_t erppm::RobotBase::getSensorDataSize() const noexcept{
    return std::accumulate(sensors.begin(), sensors.end(), 0, [](int i, const erppm::SensorBase* o){return i + o->getMeasurementDataSize();});
}

void erppm::RobotBase::reinitializeDrawables(){
    // getBody().reinitialize();
    // for(std::vector<erppm::SensorBase *>::iterator iter = sensors.begin(); iter != sensors.end(); iter = std::next(iter)){
    //     (*iter)->reinitializeDrawables();
    // }
}

void erppm::RobotBase::clearVelocity(){
    velocity = {0.0, 0.0};
    angularVelocity = 0.0;
}

void erppm::RobotBase::evolveFrom(const RobotBase& maleRobot, const RobotBase& femaleRobot){
    network.evolveFrom(maleRobot.network, femaleRobot.network);
    const double susc = 0.0001;
    {
        for
        (
            std::list<erppm::LaserSensor>::iterator
            sensorIter = getSensorList<erppm::LaserSensor>().begin(),
            maleSensorIter = getSensorList<erppm::LaserSensor>().begin(),
            femaleSensorIter = getSensorList<erppm::LaserSensor>().begin()
            ;
            sensorIter != getSensorList<erppm::LaserSensor>().end()
            ;
            sensorIter = std::next(sensorIter),
            maleSensorIter = std::next(maleSensorIter),
            femaleSensorIter = std::next(femaleSensorIter) 
        )
        {
            (*sensorIter).evolutionaryData[0] = (Jimmy::Misc::rand().binary() ? (*maleSensorIter).evolutionaryData[0] : (*femaleSensorIter).evolutionaryData[0]) + Jimmy::Misc::rand().decimal(-susc,susc);
            if((*sensorIter).evolutionaryData[0] > glm::pi<float>()){
                (*sensorIter).evolutionaryData[0] = -glm::two_pi<float>() + (*sensorIter).evolutionaryData[0];
            }   
            else if((*sensorIter).evolutionaryData[0] < -glm::pi<float>()){
                (*sensorIter).evolutionaryData[0] = glm::two_pi<float>() + (*sensorIter).evolutionaryData[0];
            }
            (*sensorIter).evolutionaryData[1] = (Jimmy::Misc::rand().binary() ? (*maleSensorIter).evolutionaryData[1] : (*femaleSensorIter).evolutionaryData[1]) + Jimmy::Misc::rand().decimal(-susc,susc);
            if((*sensorIter).evolutionaryData[1] > glm::half_pi<float>()){
                (*sensorIter).evolutionaryData[1] = glm::half_pi<float>();
            }   
            else if((*sensorIter).evolutionaryData[1] < -glm::half_pi<float>()){
                (*sensorIter).evolutionaryData[1] = -glm::half_pi<float>();
            }
            
            (*sensorIter).positionAtRobot[0] = glm::cos((*sensorIter).evolutionaryData[0]);
            (*sensorIter).positionAtRobot[1] = glm::sin((*sensorIter).evolutionaryData[0]);
            (*sensorIter).rotationAtRobot[2] = (*sensorIter).evolutionaryData[1] + (*sensorIter).evolutionaryData[0];
        } 
    }
}